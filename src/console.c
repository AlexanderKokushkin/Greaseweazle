/*
 * console.c
 * 
 * printf-style interface to USART1.
 * 
 * Written & released by Keir Fraser <keir.xen@gmail.com>
 * 
 * This is free and unencumbered software released into the public domain.
 * See the file COPYING for more details, or visit <http://unlicense.org>.
 */

#ifndef BAUD
#define BAUD 3000000 /* 3Mbaud */
#endif

#define STM32F7_DISCO 1

#if STM32F == 1
#define PCLK SYSCLK
#define usart usart1
#define USART_IRQ 37
#elif STM32F7_DISCO == 1
#define PCLK (APB1_MHZ * 1000000)
#define usart usart2
#define USART_IRQ 38
#elif STM32F == 7
#define PCLK (APB2_MHZ * 1000000)
#define usart usart1
#define USART_IRQ 37
#endif

static void ser_putc(uint8_t c)
{
#if STM32F == 1
    while (!(usart->sr & USART_SR_TXE))
        cpu_relax();
    usart->dr = c;
#elif STM32F == 7
    while (!(usart->isr & USART_ISR_TXE))
        cpu_relax();
    usart->tdr = c;
#endif
}

int vprintk(const char *format, va_list ap)
{
    static char str[128];
    unsigned int flags;
    char *p, c;
    int n;

    IRQ_global_save(flags);

    n = vsnprintf(str, sizeof(str), format, ap);

    p = str;
    while ((c = *p++) != '\0') {
        switch (c) {
        case '\r': /* CR: ignore as we generate our own CR/LF */
            break;
        case '\n': /* LF: convert to CR/LF (usual terminal behaviour) */
            ser_putc('\r');
            /* fall through */
        default:
            ser_putc(c);
            break;
        }
    }

    IRQ_global_restore(flags);

    return n;
}

int printk(const char *format, ...)
{
    va_list ap;
    int n;

    va_start(ap, format);
    n = vprintk(format, ap);
    va_end(ap);

    return n;
}

void console_init(void)
{
    /* Turn on the clocks. */
    rcc->apb2enr |= RCC_APB2ENR_USART1EN;
    peripheral_clock_delay();

    /* Enable TX pin (PA9) for USART output, RX pin (PA10) as input. */
#if STM32F == 1
    gpio_configure_pin(gpioa, 9, AFO_pushpull(_10MHz));
    gpio_configure_pin(gpioa, 10, GPI_pull_up);
#elif STM32F7_DISCO == 1
    rcc->apb1enr |= RCC_APB1ENR_USART2EN;
    peripheral_clock_delay();
    gpio_set_af(gpioa, 2, 7);
    gpio_set_af(gpioa, 3, 7);
    gpio_configure_pin(gpioa, 2, AFO_pushpull(IOSPD_MED));
    gpio_configure_pin(gpioa, 3, AFI(PUPD_up));
#elif STM32F == 7
    gpio_set_af(gpioa, 9, 7);
    gpio_set_af(gpioa, 10, 7);
    gpio_configure_pin(gpioa, 9, AFO_pushpull(IOSPD_MED));
    gpio_configure_pin(gpioa, 10, AFI(PUPD_up));
#endif

    /* BAUD, 8n1. */
    usart->brr = PCLK / BAUD;
    usart->cr1 = (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);
}

/* Debug helper: if we get stuck somewhere, calling this beforehand will cause 
 * any serial input to cause a crash dump of the stuck context. */
void console_crash_on_input(void)
{
#if STM32F == 1
    (void)usart->dr; /* clear UART_SR_RXNE */
#elif STM32F == 7
    usart->rqr = USART_RQR_RXFRQ; /* clear ISR_RXNE */
    usart->icr = USART_ICR_ORECF; /* clear ISR_ORE */
#endif
    usart->cr1 |= USART_CR1_RXNEIE;
    IRQx_set_prio(USART_IRQ, RESET_IRQ_PRI);
    IRQx_enable(USART_IRQ);
}

/*
 * Local variables:
 * mode: C
 * c-file-style: "Linux"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
