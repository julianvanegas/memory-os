#include "domain/physical_memory.h"
#include <algorithm>
#include <stdexcept>

namespace vmsim::domain {

PhysicalMemory::PhysicalMemory(size_t totalBytes)
    : frameCount_(totalBytes / PAGE_SIZE),
      bytes_(frameCount_ * PAGE_SIZE, 0),
      frameOwner_(frameCount_) {
    if (totalBytes < PAGE_SIZE || totalBytes % PAGE_SIZE != 0) {
        throw std::invalid_argument("PhysicalMemory: el tamano debe ser multiplo de PAGE_SIZE");
    }
    for (uint32_t f = 0; f < frameCount_; ++f) {
        freeFrames_.push(f);
    }
}

uint32_t PhysicalMemory::allocateFrame(VPN owner) {
    if (freeFrames_.empty()) {
        throw std::runtime_error("PhysicalMemory: no hay frames libres disponibles");
    }
    uint32_t frame = freeFrames_.front();
    freeFrames_.pop();
    frameOwner_[frame] = owner;
    return frame;
}

void PhysicalMemory::freeFrame(uint32_t frame) {
    if (frame >= frameCount_) {
        throw std::out_of_range("PhysicalMemory: frame invalido");
    }
    frameOwner_[frame].reset();
    freeFrames_.push(frame);
}

std::optional<VPN> PhysicalMemory::ownerOf(uint32_t frame) const {
    if (frame >= frameCount_) {
        throw std::out_of_range("PhysicalMemory: frame invalido");
    }
    return frameOwner_[frame];
}

uint8_t* PhysicalMemory::pointerTo(uint32_t frame, uint32_t offset) {
    if (frame >= frameCount_ || offset >= PAGE_SIZE) {
        throw std::out_of_range("PhysicalMemory: direccion fisica invalida");
    }
    return &bytes_[static_cast<size_t>(frame) * PAGE_SIZE + offset];
}

void PhysicalMemory::zeroFrame(uint32_t frame) {
    if (frame >= frameCount_) {
        throw std::out_of_range("PhysicalMemory: frame invalido");
    }
    auto begin = bytes_.begin() + static_cast<long>(frame) * PAGE_SIZE;
    std::fill(begin, begin + PAGE_SIZE, 0);
}

} // namespace vmsim::domain
