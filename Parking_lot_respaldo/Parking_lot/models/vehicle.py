from enum import Enum
from dataclasses import dataclass


class VehicleType(Enum):
    CAR = "Car"
    MOTORCYCLE = "Motorcycle"


@dataclass
class Vehicle:
    """Abstracción base de un vehículo."""
    _plate: str
    _type: VehicleType

    def __init__(self, plate: str, vehicle_type: VehicleType):
        if not plate or not plate.strip():
            raise ValueError("Las placas no pueden estar vacías.")
        self._plate = plate.upper().strip()
        self._type = vehicle_type

    def get_plate(self) -> str:
        return self._plate

    def get_type(self) -> VehicleType:
        return self._type

    def __str__(self):
        return f"{self._type.value}({self._plate})"


class Car(Vehicle):
    """Subtipo: automóvil."""
    def __init__(self, plate: str):
        super().__init__(plate, VehicleType.CAR)


class Motorcycle(Vehicle):
    """Subtipo: motocicleta."""
    def __init__(self, plate: str):
        super().__init__(plate, VehicleType.MOTORCYCLE)
