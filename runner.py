import traci
import traci.constants as tc

# Start SUMO with the TraCI interface
sumoCmd = ["sumo-gui", "-c", "testing5.sumocfg"]
traci.start(sumoCmd)

# Open a file to write speed and position data
with open("vehicle_data.csv", "w") as file:
    # Write the header row
    file.write("Step,Vehicle_ID,Speed(m/s),Position(x,y)\n")

    # Simulation loop
    step = 0
    while step < 1000:  # Run for 1000 simulation steps
        traci.simulationStep()
        vehicle_list = traci.vehicle.getIDList()
        
        for vehicle_id in vehicle_list:
            speed = traci.vehicle.getSpeed(vehicle_id)
            position = traci.vehicle.getPosition(vehicle_id)  # Returns (x, y) coordinates
            file.write(f"{step},{vehicle_id},{speed},{position[0]},{position[1]}\n")
        
        step += 1

# Close the simulation
traci.close()
