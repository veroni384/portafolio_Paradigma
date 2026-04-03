"""
cli.py  –  Menú de consola para el Simulador de Estacionamiento
Sesión 1: operaciones básicas (entrada, salida, ocupación, tickets activos)
Sesión 2: + selección de política de cobro (polimorfismo), manejo de errores
"""
from datetime import datetime
from models.vehicle import Car, Motorcycle, VehicleType
from models.parking_lot import ParkingLot
from models.rates import HourlyRatePolicy, FlatRatePolicy, DiscountedMotoPolicy


# ── Helpers de presentación ────────────────────────────────────────────────────

def _sep(char="─", n=55):
    print(char * n)

def _titulo(texto: str):
    _sep()
    print(f"  {texto}")
    _sep()

def _menu(lot: ParkingLot):
    _titulo("  SIMULADOR DE ESTACIONAMIENTO")
    occ = lot.get_occupancy()
    print(f"  Política activa : {lot.get_policy()}")
    print(f"  Ocupación       : {occ['occupied']}/{occ['total']} | Libres: {occ['free']}")
    _sep("·")
    print("  1. Registrar entrada")
    print("  2. Registrar salida")
    print("  3. Ver ocupación detallada")
    print("  4. Ver tickets activos")
    print("  5. Cambiar política de cobro  ← SESIÓN 2")
    print("  6. Salir")
    _sep()


# ── Operaciones ────────────────────────────────────────────────────────────────

def registrar_entrada(lot: ParkingLot):
    _titulo("REGISTRAR ENTRADA")
    placas = input("  Placas del vehículo: ").strip()
    if not placas:
        print("    Las placas no pueden estar vacías.")
        return

    print("  Tipo de vehículo:")
    print("    1. Auto (Car)")
    print("    2. Motocicleta (Motorcycle)")
    opcion = input("  Elige (1/2): ").strip()

    if opcion == "1":
        vehicle = Car(placas)
    elif opcion == "2":
        vehicle = Motorcycle(placas)
    else:
        print("    Tipo de vehículo inválido.")
        return

    try:
        ticket = lot.enter(vehicle, datetime.now())
        print(f"\n    Entrada registrada:")
        print(f"      Ticket  : #{ticket.get_id()}")
        print(f"      Vehículo: {vehicle}")
        print(f"      Spot    : {ticket.get_spot().get_id()}")
        print(f"      Hora    : {ticket.get_entry_time().strftime('%H:%M:%S')}")
    except ValueError as e:
        print(f"\n    {e}")


def registrar_salida(lot: ParkingLot):
    _titulo("REGISTRAR SALIDA")
    raw = input("  Número de ticket: ").strip()
    if not raw.isdigit():
        print("  ⚠  El ID del ticket debe ser un número entero.")
        return

    ticket_id = int(raw)

    activos = {t.get_id(): t for t in lot.get_active_tickets()}
    if ticket_id not in activos:
        print(f"\n    Ticket #{ticket_id} no encontrado o ya fue cerrado.")
        return

    ticket = activos[ticket_id]
    print(f"\n  Vehículo : {ticket.get_vehicle()}")
    print(f"  Spot     : {ticket.get_spot().get_id()}")
    print(f"  Entrada  : {ticket.get_entry_time().strftime('%H:%M:%S')}")
    print(f"  Política : {lot.get_policy()}")

    try:
        costo = lot.exit(ticket_id, datetime.now())
        print(f"\n    Salida registrada:")
        print(f"      Costo cobrado  : ${costo:.2f}")
        print(f"      Total recaudado: ${lot.get_total_revenue():.2f}")
    except ValueError as e:
        print(f"\n    {e}")


def ver_ocupacion(lot: ParkingLot):
    _titulo("OCUPACIÓN DEL ESTACIONAMIENTO")
    occ = lot.get_occupancy()
    print(f"  Total   : {occ['total']} lugares")
    print(f"  Libres  : {occ['free']}")
    print(f"  Ocupados: {occ['occupied']}")
    _sep("·")
    print("  Detalle de spots:")
    for spot in lot.get_spots():
        icono = "" if spot.is_occupied() else ""
        print(f"    {icono}  {spot}")


def ver_tickets_activos(lot: ParkingLot):
    _titulo("TICKETS ACTIVOS")
    tickets = lot.get_active_tickets()
    if not tickets:
        print("  (No hay vehículos en el estacionamiento)")
        return
    for t in tickets:
        print(f"  • {t}")


def cambiar_politica(lot: ParkingLot):
    """
    SESIÓN 2 — Polimorfismo en acción:
    el usuario elige qué implementación de RatePolicy usar.
    ParkingLot no cambia nada; solo se inyecta otra política.
    """
    _titulo("CAMBIAR POLÍTICA DE COBRO  (Polimorfismo)")
    print("  Políticas disponibles:")
    print("  1. HourlyRatePolicy  — por hora, distinto para auto y moto")
    print("  2. FlatRatePolicy    — tarifa fija sin importar tiempo ni vehículo")
    print("  3. DiscountedMoto    — por hora, motos con 50 % de descuento")
    opcion = input("  Elige (1/2/3): ").strip()

    if opcion == "1":
        nueva = HourlyRatePolicy(car_rate=20.0, moto_rate=10.0)
    elif opcion == "2":
        try:
            monto = float(input("  Tarifa fija ($): ").strip())
        except ValueError:
            print("    Valor inválido.")
            return
        nueva = FlatRatePolicy(flat_amount=monto)
    elif opcion == "3":
        nueva = DiscountedMotoPolicy(car_rate=20.0, discount=0.5)
    else:
        print("    Opción inválida.")
        return

    lot.set_policy(nueva)
    print(f"\n    Política actualizada: {lot.get_policy()}")
    print("      (Los tickets activos se cobrarán con esta política al salir)")


# ── Bucle principal ────────────────────────────────────────────────────────────

def run():
    lot = ParkingLot.create_default()
    print("\n  Estacionamiento inicializado con 10 spots.")
    print(f"  Política inicial: {lot.get_policy()}")

    while True:
        print()
        _menu(lot)
        opcion = input("  Elige una opción: ").strip()

        if opcion == "1":
            registrar_entrada(lot)
        elif opcion == "2":
            registrar_salida(lot)
        elif opcion == "3":
            ver_ocupacion(lot)
        elif opcion == "4":
            ver_tickets_activos(lot)
        elif opcion == "5":
            cambiar_politica(lot)
        elif opcion == "6":
            print("\n  ¡Hasta luego! \n")
            break
        else:
            print("    Opción no válida. Elige entre 1 y 6.")

        input("\n  [Enter para continuar...]")


if __name__ == "__main__":
    run()