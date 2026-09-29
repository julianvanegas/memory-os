#pragma once
#include <cstdint>

namespace vmsim::domain {

// Layout de la direccion virtual de 32 bits: PT1(10b) | PT2(10b) | Offset(12b).
// PAGE_SIZE queda fija en tiempo de compilacion porque el split 10/10/12 solo
// suma 32 bits para paginas de 4KB; si se quisiera otro tamano de pagina habria
// que recalcular PT1_BITS/PT2_BITS/OFFSET_BITS en conjunto.
constexpr uint32_t OFFSET_BITS = 12;
constexpr uint32_t PT2_BITS = 10;
constexpr uint32_t PT1_BITS = 10;
constexpr uint32_t PAGE_SIZE = 1u << OFFSET_BITS;      // 4096
constexpr uint32_t PT_ENTRIES = 1u << PT2_BITS;        // 1024

using VPN = uint32_t;

struct VirtualAddress {
    uint32_t raw;

    explicit VirtualAddress(uint32_t address) : raw(address) {}

    uint32_t pt1Index() const {
        return (raw >> (OFFSET_BITS + PT2_BITS)) & (PT_ENTRIES - 1);
    }

    uint32_t pt2Index() const {
        return (raw >> OFFSET_BITS) & (PT_ENTRIES - 1);
    }

    uint32_t offset() const {
        return raw & (PAGE_SIZE - 1);
    }

    VPN vpn() const {
        return raw >> OFFSET_BITS;
    }

    static VirtualAddress fromVpn(VPN vpn) {
        return VirtualAddress(vpn << OFFSET_BITS);
    }
};

} // namespace vmsim::domain
