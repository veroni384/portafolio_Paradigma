from enum import Enum
from datetime import datetime
from models.vehicle import Vehicle
from models.spot import ParkingSpot


class TicketStatus(Enum):
    ACTIVE = "ACTIVE"
    CLOSED = "CLOSED"


class Ticket:
    """Representa la estadía de un vehículo en el estacionamiento."""

    def __init__(
        self,
        ticket_id: int,
        vehicle: Vehicle,
        spot: ParkingSpot,
        entry_time: datetime,
    ):
        self._ticket_id = ticket_id
        self._vehicle = vehicle
        self._spot = spot
        self._entry_time = entry_time
        self._exit_time: datetime | None = None
        self._status = TicketStatus.ACTIVE

    # ── Getters ────────────────────────────────────────────────────────────
    def get_id(self) -> int:
        return self._ticket_id

    def get_vehicle(self) -> Vehicle:
        return self._vehicle

    def get_spot(self) -> ParkingSpot:
        return self._spot

    def get_entry_time(self) -> datetime:
        return self._entry_time

    def get_exit_time(self) -> datetime | None:
        return self._exit_time

    def get_status(self) -> TicketStatus:
        return self._status

    # ── Comportamiento ─────────────────────────────────────────────────────
    def close(self, exit_time: datetime) -> None:
        """Cierra el ticket registrando la hora de salida."""
        if self._status == TicketStatus.CLOSED:
            raise ValueError(f"El ticket #{self._ticket_id} ya fue cerrado.")
        if exit_time < self._entry_time:
            raise ValueError("La hora de salida no puede ser anterior a la entrada.")
        self._exit_time = exit_time
        self._status = TicketStatus.CLOSED

    def get_duration_hours(self) -> float:
        """Devuelve la duración en horas (mínimo 1 hora)."""
        if self._exit_time is None:
            raise ValueError("El ticket aún está activo; no tiene hora de salida.")
        delta = self._exit_time - self._entry_time
        hours = delta.total_seconds() / 3600
        return max(hours, 1.0)   # mínimo cobro: 1 hora

    def __str__(self):
        spot_id = self._spot.get_id()
        entry = self._entry_time.strftime("%H:%M")
        return (
            f"Ticket#{self._ticket_id} | {self._vehicle} | "
            f"Spot={spot_id} | Entrada={entry} | {self._status.value}"
        )
