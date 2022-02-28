obj-$(CONFIG_HYPERV)		+= hv_vmbus.o
obj-$(CONFIG_HYPERV_UTILS)	+= hv_utils.o
obj-$(CONFIG_HID_BASE_HV)	+= hid-base-hv.o
obj-$(CONFIG_HID_HYPERV_MOUSE)	+= hid-hyperv.o


hv_vmbus-objs := vmbus_drv.o  hv.o connection.o channel.o channel_mgmt.o \
	ring_buffer.o hv_compat.o
hv_utils-objs := hv_util.o hv_kvp.o
hid-base-hv-objs := hid-core.o hid-input.o hid-debug.o

