"""
app.py  –  Controller (Flask)
Sesión 3: interfaz web MVC simple
  Model      -> models/  (ya implementado en sesiones 1 y 2)
  View       -> templates/
  Controller -> este archivo (rutas Flask)
"""
from datetime import datetime
from flask import Flask, render_template, request, redirect, url_for, flash
from models.vehicle import Car, Motorcycle
from models.parking_lot import ParkingLot
from models.rates import HourlyRatePolicy, FlatRatePolicy, DiscountedMotoPolicy

app = Flask(__name__)
app.secret_key = "parking_lot_secret"

# ── Estado en memoria (el mismo modelo de sesiones 1 y 2) ─────────────────────
lot = ParkingLot.create_default()


@app.route("/")
def dashboard():
    occ = lot.get_occupancy()
    tickets = lot.get_active_tickets()
    spots = lot.get_spots()
    return render_template(
        "dashboard.html",
        occ=occ,
        tickets=tickets,
        spots=spots,
        policy=lot.get_policy(),
        revenue=lot.get_total_revenue(),
    )


@app.route("/entry", methods=["GET"])
def entry_get():
    return render_template("entry.html")


@app.route("/entry", methods=["POST"])
def entry_post():
    plate = request.form.get("plate", "").strip().upper()
    vtype = request.form.get("vehicle_type", "")

    if not plate:
        flash("Las placas no pueden estar vacías.", "error")
        return redirect(url_for("entry_get"))
    if vtype not in ("Car", "Motorcycle"):
        flash("Tipo de vehículo inválido.", "error")
        return redirect(url_for("entry_get"))

    try:
        vehicle = Car(plate) if vtype == "Car" else Motorcycle(plate)
        ticket = lot.enter(vehicle, datetime.now())
        flash(
            f"Entrada registrada — Ticket #{ticket.get_id()} | Spot {ticket.get_spot().get_id()}",
            "success",
        )
    except ValueError as e:
        flash(str(e), "error")

    return redirect(url_for("dashboard"))


@app.route("/exit", methods=["GET"])
def exit_get():
    tickets = lot.get_active_tickets()
    return render_template("exit.html", tickets=tickets)


@app.route("/exit", methods=["POST"])
def exit_post():
    raw = request.form.get("ticket_id", "").strip()
    if not raw.isdigit():
        flash("El ID del ticket debe ser un número.", "error")
        return redirect(url_for("exit_get"))

    ticket_id = int(raw)
    try:
        costo = lot.exit(ticket_id, datetime.now())
        flash(
            f"Salida registrada — Ticket #{ticket_id} | Costo: ${costo:.2f}",
            "success",
        )
    except ValueError as e:
        flash(str(e), "error")

    return redirect(url_for("dashboard"))


@app.route("/policy", methods=["POST"])
def change_policy():
    choice = request.form.get("policy", "")
    if choice == "hourly":
        lot.set_policy(HourlyRatePolicy(20.0, 10.0))
    elif choice == "flat":
        try:
            amount = float(request.form.get("flat_amount", 50))
        except ValueError:
            amount = 50.0
        lot.set_policy(FlatRatePolicy(amount))
    elif choice == "discounted":
        lot.set_policy(DiscountedMotoPolicy(20.0, 0.5))
    else:
        flash("Política inválida.", "error")
        return redirect(url_for("dashboard"))

    flash(f"Política actualizada: {lot.get_policy()}", "success")
    return redirect(url_for("dashboard"))


if __name__ == "__main__":
    app.run(debug=True)