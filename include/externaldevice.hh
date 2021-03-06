// Copyright (C) 2013, Julian Stecklina <jsteckli@os.inf.tu-dresden.de>
// Economic rights: Technische Universitaet Dresden (Germany)

// This file is part of sv3.

// sv3 is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

// sv3 is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License version 2 for more details.

#pragma once

#include <cstdint>

namespace Switch {

  class Session;

  /// A model for a PCI device implemented outside of qemu.
  class ExternalDevice {
  protected:
    Session &_session;

    ExternalDevice(Session &session) : _session(session) { }

  public:

    virtual void get_device_info(uint16_t &vendor_id,    uint16_t &device_id,
				 uint16_t &subsystem_id, uint16_t &subsystem_vendor_id) = 0;

    virtual void get_bar_info   (uint8_t   bar_no, uint32_t &size) = 0;

    virtual void get_irq_info   (uint8_t &msix_vectors) = 0;

    virtual void get_hotspot    (uint8_t  &hotspot_bar_no,
				 uint16_t &hotspot_addr,
				 uint8_t  &hotspot_size,
				 int      &hotspot_fd) = 0;

    virtual void get_msix_info  (int fd, int index,
				 bool &valid, bool &more) = 0;


    virtual uint64_t io_read (unsigned bar_no,
			      uint64_t addr,
			      unsigned size) = 0;

    virtual void io_write(unsigned bar_no,
			  uint64_t addr,
			  unsigned size,
			  uint64_t val,
			  bool &irqs_changed) = 0;

    virtual void reset() = 0;
  };

}

// EOF
