/*
 ============================================================================
 Name        : main.c
 Author      : Tim Wilkinson
 Version     :
 Copyright   : (c) Tim Wilkinson
 Description : Hello World in C
 ============================================================================
 */

#include <softdevice_handler.h>
#if defined(CONFIG_S132)
#include <softdevice_handler_appsh.h>
#endif
#include <app_scheduler.h>
#include <pstorage.h>

#include "services/gpio.h"
#include "services/timer.h"
#include "services/scheduler.h"
#include "services/advertising.h"
#include "services/gap.h"
#include "services/devinfo.h"
#include "services/connparams.h"

#include "homekit/homekit.h"
#include "homekit/services/switch.h"
#include "homekit/statistics.h"



/**@brief Function for error handling, which is called when an error has occurred.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name.
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    // This call can be used for debug purposes during application development.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover on reset.
   NVIC_SystemReset();
}

/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t* p_ble_evt)
{
  ble_conn_params_on_ble_evt(p_ble_evt);
  homekit_ble_event(p_ble_evt);
}

/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in]   sys_evt   System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
  pstorage_sys_event_handler(sys_evt);
}

/*
 * The beginning ...
 */
int main(void)
{
	uint32_t err_code;

	gpio_init();
	scheduler_init();
	// NB: If I put this init macro in its one function in its own service file, things stop working. No idea why :-(
  APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

	// Initialize the SoftDevice handler module.
#if defined(SOFTDEVICE_HANDLER_APPSH_INIT)
	SOFTDEVICE_HANDLER_APPSH_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, true);
#else
  SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, true);
#endif

	// Enable BLE stack
	ble_enable_params_t ble_enable_params =
	{
		.gatts_enable_params =
		 {
       .service_changed = 1,
       .attr_tab_size   = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT
		 }
	};
	err_code = sd_ble_enable(&ble_enable_params);
	APP_ERROR_CHECK(err_code);

	// Register with the SoftDevice handler module for BLE events.
	err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
	APP_ERROR_CHECK(err_code);

	// Register with the SoftDevice handler module for System events.
	err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
	APP_ERROR_CHECK(err_code);

	// Power setup
	sd_ble_gap_tx_power_set(TX_POWER);

	// Service setup
	pstorage_init();
	gap_init();
	devinfo_init();
	advertising_init();
	conn_params_init();

	homekit_init();
	service_switch_init();

#if defined(INCLUDE_STATISTICS)
	statistics_init();
#endif

	advertising_start();
	while (1)
	{
		app_sched_execute();
		homekit_execute();
		err_code = sd_app_evt_wait();
		APP_ERROR_CHECK(err_code);
	}

	return 0;
}
