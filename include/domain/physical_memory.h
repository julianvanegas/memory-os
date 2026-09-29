#pragma once
#include <cstdint>
#include <optional>
#include <queue>
#include <vector>
#include "domain/types.h"

namespace vmsim::domain {

// Modela la RAM simulada: el buffer de bytes, el pool de frames libres y el
// mapa frame -> VPN que permite, al momento de evictar, saber que pagina
// virtual habia que invalidar en la tabla de paginas.
class PhysicalMemory {
public:
    explicit PhysicalMemory(size_t totalBytes);

    size_t frameCount() const { return frameCount_; }
    bool hasFreeFrame() const { return !freeFrames_.empty(); }

    uint32_t allocateFrame(VPN owner);
    void freeFrame(uint32_t frame);
    std::optional<VPN> ownerOf(uint32_t frame) const;

    uint8_t* pointerTo(uint32_t frame, uint32_t offset);
    void zeroFrame(uint32_t frame);

private:
    size_t frameCount_;
    std::vector<uint8_t> bytes_;
    std::queue<uint32_t> freeFrames_;
    std::vector<std::optional<VPN>> frameOwner_;
};

} // namespace vmsim::domain
