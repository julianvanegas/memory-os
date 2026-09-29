#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include "domain/types.h"

namespace vmsim::domain {

struct Allocation {
    uint32_t base;
    uint64_t size;
    bool freed = false;
};

// Bump allocator determinista sobre el espacio virtual de 32 bits: cada
// alloc() crece la direccion base monotonamente desde 0, redondeando a
// multiplos de PAGE_SIZE. free() marca la asignacion como liberada pero el
// rango virtual NO se reutiliza (limitacion conocida, documentada en el
// reporte) - simplifica el manejo de fragmentacion.
class VirtualAllocator {
public:
    // Devuelve la direccion base asignada, o std::nullopt si no hay espacio
    // virtual restante (excede 32 bits) o bytes == 0.
    std::optional<uint32_t> alloc(uint32_t bytes);

    // Si baseAddress corresponde a la base de una asignacion viva, la marca
    // liberada y devuelve una copia de la asignacion (para que el llamador
    // sepa cuantas paginas desmapear). Devuelve std::nullopt si es invalida.
    std::optional<Allocation> free(uint32_t baseAddress);

    bool isAllocated(uint32_t address) const;

private:
    std::vector<Allocation> allocations_;
    uint64_t nextFreeVA_ = 0;
};

} // namespace vmsim::domain
