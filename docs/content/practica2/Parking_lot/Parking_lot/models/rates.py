from abc import ABC, abstractmethod
from typing import Protocol, runtime_checkable
from models.vehicle import Vehicle, VehicleType


# ── Contrato (Protocol) ────────────────────────────────────────────────────────
@runtime_checkable
class RatePolicy(Protocol):
    """
    Contrato para cualquier política de cobro.
    Usando typing.Protocol permite implementaciones intercambiables
    sin necesidad de heredar explícitamente (duck typing estructural).
    """
    def calculate(self, hours: float, vehicle: Vehicle) -> float: ...
    def __str__(self) -> str: ...


# ── Implementación 1: Tarifa por hora (diferente para auto y moto) ─────────────
class HourlyRatePolicy:
    """
    Polimorfismo por tarifa Y por vehículo:
    cobra distinto según tipo de vehículo y tiempo transcurrido.
    """

    def __init__(self, car_rate: float = 20.0, moto_rate: float = 10.0):
        if car_rate < 0 or moto_rate < 0:
            raise ValueError("Las tarifas no pueden ser negativas.")
        self._car_rate = car_rate
        self._moto_rate = moto_rate

    def calculate(self, hours: float, vehicle: Vehicle) -> float:
        rate = (
            self._car_rate
            if vehicle.get_type() == VehicleType.CAR
            else self._moto_rate
        )
        return round(hours * rate, 2)

    def __str__(self):
        return f"HourlyRate(auto=${self._car_rate}/h, moto=${self._moto_rate}/h)"


# ── Implementación 2: Tarifa plana (misma tarifa sin importar tiempo) ──────────
class FlatRatePolicy:
    """
    Polimorfismo por tarifa: cobra una cantidad fija sin importar las horas.
    Demuestra que se puede cambiar la política sin tocar ParkingLot.
    """

    def __init__(self, flat_amount: float = 50.0):
        if flat_amount < 0:
            raise ValueError("La tarifa plana no puede ser negativa.")
        self._flat_amount = flat_amount

    def calculate(self, hours: float, vehicle: Vehicle) -> float:
        # Misma tarifa para cualquier vehículo y cualquier tiempo
        return round(self._flat_amount, 2)

    def __str__(self):
        return f"FlatRate(${self._flat_amount} fijo)"


# ── Implementación 3: Tarifa por hora con descuento para motos ────────────────
class DiscountedMotoPolicy:
    """
    Extensión controlada: autos pagan tarifa normal,
    motos obtienen 50 % de descuento sobre la tarifa de auto.
    """

    def __init__(self, car_rate: float = 20.0, discount: float = 0.5):
        if not (0 < discount <= 1):
            raise ValueError("El descuento debe estar entre 0 y 1.")
        self._car_rate = car_rate
        self._discount = discount

    def calculate(self, hours: float, vehicle: Vehicle) -> float:
        rate = (
            self._car_rate
            if vehicle.get_type() == VehicleType.CAR
            else self._car_rate * (1 - self._discount)
        )
        return round(hours * rate, 2)

    def __str__(self):
        pct = int(self._discount * 100)
        return f"DiscountedMoto(auto=${self._car_rate}/h, moto={pct}% desc.)"