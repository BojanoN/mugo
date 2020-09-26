#include "pic.h"
#include <arch/interrupt.h>
#include <arch/util.h>

#define MASTER_PIC                0x20
#define SLAVE_PIC                 0xA0
#define MASTER_PIC_COMMAND        MASTER_PIC
#define MASTER_PIC_DATA           (MASTER_PIC + 1)
#define SLAVE_PIC_COMMAND         SLAVE_PIC
#define SLAVE_PIC_DATA            (SLAVE_PIC + 1)
#define SLAVE_PIC_MASTER_IRQ_LINE 2

#define MASTER_PIC_IRQ_OFFSET 0x20
#define SLAVE_PIC_IRQ_OFFSET  0x28

// End-of-interrupt command
#define PIC_EOI      0x20
#define PIC_READ_IRR 0x0A
#define PIC_READ_ISR 0x0B

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

#define SLAVE_IRQNUM_BEGIN 40
#define IRQ_OFFSET         32

void PIC_sendEOI(uint16_t irqnum)
{
    if (irqnum >= SLAVE_IRQNUM_BEGIN) {
        outb(SLAVE_PIC_COMMAND, PIC_EOI);
    }
    outb(MASTER_PIC_COMMAND, PIC_EOI);
}

void PIC_init(void)
{
    disable_interrupts();

    uint8_t init_seq_flags = ICW1_INIT | ICW1_ICW4;

    uint8_t master_mask, slave_mask;

    master_mask = (0xFF ^ (1 << SLAVE_PIC_MASTER_IRQ_LINE));
    slave_mask  = 0xFF;

    // Start initialization sequence
    outb(MASTER_PIC_COMMAND, init_seq_flags);
    outb(SLAVE_PIC_COMMAND, init_seq_flags);

    // Remap PIC interrupts
    outb(MASTER_PIC_DATA, MASTER_PIC_IRQ_OFFSET);
    outb(SLAVE_PIC_DATA, SLAVE_PIC_IRQ_OFFSET);

    // Set slave PIC to master's IRQ2
    outb(MASTER_PIC_DATA, (1 << SLAVE_PIC_MASTER_IRQ_LINE));
    outb(SLAVE_PIC_DATA, 0x2);

    outb(MASTER_PIC_DATA, ICW4_8086);
    outb(SLAVE_PIC_DATA, ICW4_8086);

    outb(MASTER_PIC_DATA, master_mask);
    outb(SLAVE_PIC_DATA, slave_mask);

    enable_interrupts();
}

void PIC_mask_irq(uint16_t irqnum)
{
    uint8_t  val  = 0;
    uint16_t port = MASTER_PIC_DATA;

    irqnum -= IRQ_OFFSET;

    // Check if slave PIC caused the interrupt
    if (irqnum >= 8) {
        irqnum -= 8;
        port = SLAVE_PIC_DATA;
    }

    // Read existing mask
    val = inb(port);
    val |= (1 << irqnum);

    outb(port, val);
}

void PIC_unmask_irq(uint16_t irqnum)
{
    uint8_t  val  = 0;
    uint16_t port = MASTER_PIC_DATA;

    irqnum -= IRQ_OFFSET;

    // Check if slave PIC caused the interrupt
    if (irqnum >= 8) {
        irqnum -= 8;
        port = SLAVE_PIC_DATA;
    }

    // Read existing mask
    val = inb(port);
    val &= ~(1 << irqnum);

    outb(port, val);
}

static uint16_t __pic_read_register(uint8_t cmd)
{
    uint16_t ret = 0;

    outb(MASTER_PIC_COMMAND, cmd);
    outb(SLAVE_PIC_COMMAND, cmd);

    ret = (inb(SLAVE_PIC_COMMAND) << 8) | inb(MASTER_PIC_COMMAND);

    return ret;
}

uint16_t PIC_read_ISR(void)
{
    return __pic_read_register(PIC_READ_ISR);
}

uint16_t PIC_read_IRR(void)
{
    return __pic_read_register(PIC_READ_IRR);
}

uint32_t PIC_check_spurious(void)
{
    uint16_t irq_requests = PIC_read_IRR();

    // Check for spurious IRQ on master PIC (IRQ7)
    if (irq_requests & (1 << 7)) {
        return 1;
    }
    // Check for spurious IRQ on slave PIC (IRQ15)
    if (irq_requests & (1 << 15)) {
        // Send EOI to master PIC
        outb(MASTER_PIC_COMMAND, PIC_EOI);
        return 1;
    }

    return 0;
}
