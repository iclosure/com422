
#ifndef _HR_PCOMM_PORT_TIMEOUT_H_
#define _HR_PCOMM_PORT_TIMEOUT_H_

KDEFERRED_ROUTINE      TimerWriteDpc;
KDEFERRED_ROUTINE      TimerReadDpc;

NTSTATUS SetReadTimeout(
	IN PFDOPORT_CONTEXT_EXTENSION PortContext,
	IN PIRP Irp);

NTSTATUS SetWriteTimeout(
	IN PFDOPORT_CONTEXT_EXTENSION PortContext,
	IN PIRP Irp);

#endif /* _HR_PCOMM_PORT_TIMEOUT_H_ */
