
#include <virtiodevice.hh>
#include <switch.hh>
#include <session.hh>

#include <sstream>

namespace Switch {


  void VirtioDevice::get_device_info(uint16_t &vendor_id,    uint16_t &device_id,
				     uint16_t &subsystem_id, uint16_t &subsystem_vendor_id)
  {
    vendor_id           = VIRTIO_NET_VENDOR_ID;
    device_id           = VIRTIO_NET_DEVICE_ID;
    subsystem_id        = VIRTIO_NET_SUBSYSTEM_ID;
    subsystem_vendor_id = VIRTIO_NET_SUBSYSTEM_VENDOR_ID;
  }

  void VirtioDevice::get_bar_info   (uint8_t   bar_no, uint32_t &size)
  {
    size = (bar_no == 0 ) ? (0x40 | PCI_BASE_ADDRESS_SPACE_IO) : 0;
  }

  void VirtioDevice::get_irq_info   (uint8_t &msix_vectors)
  { msix_vectors = MSIX_VECTORS; }

  void VirtioDevice::get_hotspot    (uint8_t  &bar_no,
				     uint16_t &addr,
				     uint8_t  &size,
				     int      &fd)
  {
    bar_no = 0;
    addr   = VIRTIO_PCI_QUEUE_NOTIFY;
    size   = 2;
    fd     = _session._sw.event_fd();
  }

  void VirtioDevice::get_msix_info  (int fd, int index,
				     bool &valid, bool &more)
  {
    if (index < MSIX_VECTORS) {
      if (_irq_fd[index] == 0) {
	_irq_fd[index] = fd;
	logf("MSI-X vector %u triggered by fd %u.", index, fd);
	valid = true;
      } else {
	/* Already configured */
	_session.close_fd(fd);
	valid = false;
      }
    }

    more = ((index + 1) < MSIX_VECTORS);
  }

  VirtioDevice::VirtioDevice(Session &session)
    : ExternalDevice(session),
      Port(session._sw, std::string("VirtIO ") + std::to_string(session._fd)),
      _irq_fd()
  {

  }

}

// EOF
