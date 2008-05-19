/*
 * This code handles the interrupt descriptor tables.
*/
#include <kernel.h>
#include <arch/mm.h>
#include <arch/descriptor.h>
#include <arch/irq.h>
#include <arch/idt.h>

void idle_task(void);

#define load_idt(idtr) \
	asm volatile("lidt (%0)": :"r" (idtr));

/* IDT structure */
dt_entry_t __desc_aligned IDT[256];
struct gdtr loadidt={ sizeof(IDT)-1, (uint32_t)IDT};

/* Add a new trap vector to the IDT  */
void idt_exception(void *handler, uint8_t intr)
{
	long flags;
	lock_irq(flags);
	IDT[intr].gate.selector		= __KERNEL_CS;
	IDT[intr].gate.offset_low	= (uint16_t)(((uint32_t)handler)&0xffff);
	IDT[intr].gate.offset_high 	= (uint16_t)(((uint32_t)handler)>>16);
	IDT[intr].gate.access		= D_PRESENT | D_TRAP | D_DPL3;
	unlock_irq(flags);
}

/* Add a new interrupt vector to the IDT  */
void idt_interrupt(void *handler, uint8_t intr)
{
	long flags;
	lock_irq(flags);
	IDT[intr].gate.selector		= __KERNEL_CS;
	IDT[intr].gate.offset_low	= (uint16_t)(((uint32_t)handler)&0xffff);
	IDT[intr].gate.offset_high 	= (uint16_t)(((uint32_t)handler)>>16);
	IDT[intr].gate.access		= D_PRESENT | D_INT;
	unlock_irq(flags);
}

struct {
	int fault;
	char *name;
}exc[]={
	{1, "Divide Error"},
	{1, "Debug"},
	{0, "NMI"},
	{0, "Breakpoint"},
	{0, "Overflow"},

	{1, "Bounds Check"},
	{1, "Invalid Opcode"},
	{1, "Coprocessor not available"},
	{1, "Double fault"},
	{1, "Coprocessor segment overrun"},

	{1, "Invalid TSS"},
	{1, "Segment not present"},
	{1, "Stack segment"},
	{1, "General Protection"},
	{1, "Page Fault"},

	{0, NULL},
	{1, "FPU error"},
	{1, "Alignment Check"},
	{1, "Machine Check"},
	{1, "SIMD Exception"},
};

void exc_handler(int num, int flags)
{
	printk("%s: %s\n",
		exc[num].fault ? "fault" : "trap",
		exc[num].name);	

	/* Can't handle faults just yet */
	if ( exc[num].fault ) idle_task();
}

void syscall(void)
{
	printk("syscall\n");
}

void __init idt_init(void)
{
	int count;

	for(count=0; count<256; count++)
		idt_exception(int_null, count);

	/* Standard fault/trap handlers */
	idt_exception(_exc0, 0);
	idt_exception(_exc1, 1);
	idt_exception(_exc2, 2);
	idt_exception(_exc3, 3);
	idt_exception(_exc4, 4);
	idt_exception(_exc5, 5);
	idt_exception(_exc6, 6);
	idt_exception(_exc7, 7);
	idt_exception(_exc8, 8);
	idt_exception(_exc9, 9);
	idt_exception(_exc10, 10);
	idt_exception(_exc11, 11);
	idt_exception(_exc12, 12);
	idt_exception(_exc13, 13);
	idt_exception(_exc14, 14);
	/* exception 15 is resrved */
	idt_exception(_exc16, 16);
	idt_exception(_exc17, 17);
	idt_exception(_exc18, 18);
	idt_exception(_exc19, 19);

	/* 20 -> 32 are reserved */

	/* Interrupts caused by externally
	 * generated IRQ lines */
	idt_interrupt(_irq0, 0x20);
	idt_interrupt(_irq1, 0x21);
	idt_interrupt(_irq2, 0x22);
	idt_interrupt(_irq3, 0x23);
	idt_interrupt(_irq4, 0x24);
	idt_interrupt(_irq5, 0x25);
	idt_interrupt(_irq6, 0x26);
	idt_interrupt(_irq7, 0x27);
	idt_interrupt(_irq8, 0x28);
	idt_interrupt(_irq9, 0x29);
	idt_interrupt(_irq10, 0x30);
	idt_interrupt(_irq11, 0x3a);
	idt_interrupt(_irq12, 0x3b);
	idt_interrupt(_irq13, 0x3c);
	idt_interrupt(_irq14, 0x3d);
	idt_interrupt(_irq15, 0x3e);

	/* System call */
	idt_interrupt(_syscall, 0x80);

	/* Yay - we're finished */
	load_idt(&loadidt);
}