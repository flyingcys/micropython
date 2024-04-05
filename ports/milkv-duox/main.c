#include <stdio.h>
#include <string.h>
#include "py/compile.h"
#include "py/runtime.h"

#include "csi_pin.h"
#include "csi_uart.h"

static const char *demo_single_input =
    "print('hello world!', list(x + 1 for x in range(10)), end='eol\\n')";

static const char *demo_file_input =
    "import micropython\n"
    "\n"
    "print(dir(micropython))\n"
    "\n"
    "for i in range(10):\n"
    "    print('iter {:08}'.format(i))";

static void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // Compile, parse and execute the given string.
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // Uncaught exception: print it out.
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
uint8_t _rxPin, _txPin;
int _uartNum;

csi_uart_t _uart;
extern dev_pin_map_t uart_rx_map[];
extern dev_pin_map_t uart_tx_map[];

void uart_init(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin)
{
    const dev_pin_map_t* rx_pin = target_pin_number_to_dev(rxPin, uart_rx_map, _uartNum);
    const dev_pin_map_t* tx_pin = target_pin_number_to_dev(txPin, uart_tx_map, _uartNum);

    if (rx_pin == NULL || tx_pin == NULL) {
        pr_err("pin GPIO %d or %d are not used as Uart func\n", rxPin, txPin);
        return;
    }

    if (csi_pin_set_mux(rx_pin->name, rx_pin->func)) {
        pr_err("pin GPIO %d fails to config as Uart_rx func\n", rxPin);
        return;
    }

    if (csi_pin_set_mux(tx_pin->name, tx_pin->func)) {
        pr_err("pin GPIO %d fails to config as Uart_tx func\n", txPin);
        return;
    }

    csi_uart_init(&_uart, _uartNum);

    _rxPin = rxPin;
    _txPin = txPin;

    csi_uart_format(&_uart, SERIAL_DATA(config), SERIAL_PARITY(config), SERIAL_STOP(config));
    csi_uart_baud(&_uart, baud);
}
void pre_system_init(void)
{
    irq_init();
    tick_init();
// 	pinmux_init();
	uart_init(115200, 8, 15, 14);
// 	irq_init();
// 	printf("Pre system init done\n");
}

int main(void)
{

    pre_system_init();

	// prvSetupHardware();

    // vTaskStartScheduler();

    mp_init();
    do_str(demo_single_input, MP_PARSE_SINGLE_INPUT);
    do_str(demo_file_input, MP_PARSE_FILE_INPUT);
    mp_deinit();

   return 0;
}

void nlr_jump_fail(void *val) {
    for (;;) {
    }
}