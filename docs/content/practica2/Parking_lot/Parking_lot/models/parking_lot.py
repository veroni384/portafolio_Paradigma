from datetime import datetime
from models.vehicle import Vehicle
from models.spot import ParkingSpot, SpotType
from models.ticket import Ticket, TicketStatus
from models.rates import RatePolicy, HourlyRatePolicy, FlatRatePolicy, DiscountedMotoPolicy


class ParkingLot:
    """
    Administra el estacionamiento completo:
    spots, tickets activos y recaudación total.
    Composición: contiene ParkingSpot y Ticket.
    """

    def __init__(self, policy: RatePolicy | None = None):
        self._spots: list[ParkingSpot] = []
        self._active_tickets: dict[int, Ticket] = {}
        self._policy: RatePolicy = policy or HourlyRatePolicy()
        self._next_ticket_id: int = 1
        self._total_revenue: float = 0.0

    # ── Configuración ──────────────────────────────────────────────────────
    def add_spot(self, spot: ParkingSpot) -> None:
        self._spots.append(spot)

    def set_policy(self, policy: RatePolicy) -> None:
        self._policy = policy

    def get_policy(self) -> RatePolicy:
        return self._policy

    # ── Entrada ────────────────────────────────────────────────────────────
    def enter(self, vehicle: Vehicle, now: datetime | None = None) -> Ticket:
        """
        Registra la entrada de un vehículo.
        Busca el primer spot compatible disponible.
        """
        if now is None:
            now = datetime.now()

        spot = self._find_available_spot(vehicle)
        if spot is None:
            raise ValueError(
                f"No hay lugares disponibles para {vehicle.get_type().value}."
            )

        spot.park(vehicle)
        ticket = Ticket(self._next_ticket_id, vehicle, spot, now)
        self._active_tickets[self._next_ticket_id] = ticket
        self._next_ticket_id += 1
        return ticket

    # ── Salida ─────────────────────────────────────────────────────────────
    def exit(self, ticket_id: int, now: datetime | None = None) -> float:
        """
        Registra la salida: cierra ticket, libera spot, calcula costo.
        Retorna el costo cobrado.
        """
        if now is None:
            now = datetime.now()

        ticket = self._active_tickets.get(ticket_id)
        if ticket is None:
            raise ValueError(
                f"Ticket #{ticket_id} no encontrado o ya fue cerrado."
            )

        ticket.close(now)
        hours = ticket.get_duration_hours()
        cost = self._policy.calculate(hours, ticket.get_vehicle())

        ticket.get_spot().release()
        self._total_revenue += cost
        del self._active_tickets[ticket_id]
        return cost

    # ── Consultas ──────────────────────────────────────────────────────────
    def get_occupancy(self) -> dict:
        total = len(self._spots)
        occupied = sum(1 for s in self._spots if s.is_occupied())
        return {
            "total": total,
            "occupied": occupied,
            "free": total - occupied,
        }

    def get_active_tickets(self) -> list[Ticket]:
        return list(self._active_tickets.values())

    def get_total_revenue(self) -> float:
        return self._total_revenue

    def get_spots(self) -> list[ParkingSpot]:
        return list(self._spots)

    # ── Privado ────────────────────────────────────────────────────────────
    def _find_available_spot(self, vehicle: Vehicle) -> ParkingSpot | None:
        for spot in self._spots:
            if spot.is_available_for(vehicle):
                return spot
        return None

    # ── Inicialización por defecto ─────────────────────────────────────────
    @classmethod
    def create_default(cls, policy: RatePolicy | None = None) -> "ParkingLot":
        """
        Crea un estacionamiento de ejemplo con 10 spots:
        5 para autos (A1-A5), 3 para motos (M1-M3), 2 mixtos (X1-X2).
        """
        lot = cls(policy)
        for i in range(1, 6):
            lot.add_spot(ParkingSpot(f"A{i}", SpotType.CAR))
        for i in range(1, 4):
            lot.add_spot(ParkingSpot(f"M{i}", SpotType.MOTORCYCLE))
        for i in range(1, 3):
            lot.add_spot(ParkingSpot(f"X{i}", SpotType.ANY))
        return lot