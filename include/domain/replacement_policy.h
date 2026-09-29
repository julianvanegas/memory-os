#pragma once
#include <cstdint>

namespace vmsim::domain {

// Puerto (Dependency Inversion): mmu.h y simulator_service.h dependen solo de
// esta interfaz, nunca de FifoPolicy/LruPolicy directamente. Eso permite
// intercambiar la politica de reemplazo sin tocar el resto del dominio.
class IReplacementPolicy {
public:
    virtual ~IReplacementPolicy() = default;

    // Se llama justo despues de que un fallo de pagina asigna `frame`.
    virtual void onFrameLoaded(uint32_t frame) = 0;

    // Se llama en cada hit (read o write) sobre `frame`.
    virtual void onAccess(uint32_t frame) = 0;

    // Se llama cuando hay un fallo de pagina y no hay frames libres.
    virtual uint32_t pickVictim() = 0;

    // Se llama cuando un frame se libera explicitamente (free() o eviction).
    virtual void onFrameFreed(uint32_t frame) = 0;

    virtual const char* name() const = 0;
};

} // namespace vmsim::domain
