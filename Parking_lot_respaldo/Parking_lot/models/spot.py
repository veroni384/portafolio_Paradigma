from enum import Enum
from dataclasses import dataclass, field
from models.vehicle import Vehicle, VehicleType


class SpotType(Enum):
    CAR = "Car"
    MOTORCYCLE = "Motorcycle"
    ANY = "Any"


class ParkingSpot:
    """Representa un lugar individual dentro del estacionamiento."""

    def __init__(self, spot_id: str, allowed: SpotType = SpotType.ANY):
        self._spot_id = spot_id
        self._allowed = allowed
        self._occupied = False
        self._current_vehicle: Vehicle | None = None

    # ── Getters ────────────────────────────────────────────────────────────
    def get_id(self) -> str:
        return self._spot_id

    def get_allowed(self) -> SpotType:
        return self._allowed

    def is_occupied(self) -> bool:
        return self._occupied

    def get_current_vehicle(self) -> Vehicle | None:
        return self._current_vehicle

    # ── Comportamiento ─────────────────────────────────────────────────────
    def is_available_for(self, vehicle: Vehicle) -> bool:
        """Verifica si el lugar acepta el tipo de vehículo dado."""
        if self._occupied:
            return False
        if self._allowed == SpotType.ANY:
            return True
        # Mapeo VehicleType -> SpotType
        vehicle_spot_type = SpotType(vehicle.get_type().value)
        return self._allowed == vehicle_spot_type

    def park(self, vehicle: Vehicle) -> None:
        """Estaciona un vehículo. Valida disponibilidad antes de asignar."""
        if self._occupied:
            raise ValueError(f"El lugar {self._spot_id} ya está ocupado.")
        if not self.is_available_for(vehicle):
            raise ValueError(
                f"El lugar {self._spot_id} no acepta {vehicle.get_type().value}."
            )
        self._current_vehicle = vehicle
        self._occupied = True

    def release(self) -> None:
        """Libera el lugar."""
        if not self._occupied:
            raise ValueError(f"El lugar {self._spot_id} ya estaba libre.")
        self._current_vehicle = None
        self._occupied = False

    def __str__(self):
        status = f"ocupado por {self._current_vehicle}" if self._occupied else "libre"
        return f"Spot[{self._spot_id}|{self._allowed.value}] -> {status}"
