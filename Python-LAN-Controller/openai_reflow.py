import socket
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import threading
import tkinter as tk

# Set up the UDP socket for receiving data
UDP_IP = ""
UDP_PORT = 55000
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

# Set up the target IP address and port
TARGET_IP = "192.168.0.209"
TARGET_PORT = 40292

# Set up the plot
fig, ax = plt.subplots()
ax.set_xlim(0, 360)
ax.set_ylim(0, 260)
ax.set_xlabel("Time (s)")
ax.set_ylabel("Input (°C)")
ax.grid()

# Define the function that will be called for each iteration of the animation
def animate(i):
    ax.clear()
    ax.grid()
    ax.plot(times, inputs, "r")
    ax.plot(times, setpoints, "b")
    ax.set_xlim(0, 360)
    ax.set_ylim(0, 260)
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Input (°C)")

# Define the function that will be called when the button is pressed
def press_right():
    sock.sendto(b":", (TARGET_IP, TARGET_PORT))

def press_left():
    sock.sendto(b"d", (TARGET_IP, TARGET_PORT))

# Create a button widget
button_r = tk.Button(text="BUTTON RIGHT", command=press_right)
button_l = tk.Button(text="BUTTON LEFT", command=press_left)
button_l.pack()
button_r.pack()

# Define the function that will receive the data from the socket
def receive_data():
    global times, inputs, setpoints
    while True:
        # Receive data from the socket
        data, _ = sock.recvfrom(1024)

        # Ignore the packet if it doesn't match the expected scheme
        if not data.startswith(b"(") or not data.endswith(b")"):
            continue

        # Extract the values from the received data
        packet = data.decode()[1:-1].split(",")
        if len(packet) != 4:
            continue

        try:
            # Extract the values from the received data
            time, setpoint, input, output = map(float, packet)
        except ValueError:
            continue

        # Append the values to the lists
        times.append(time)
        inputs.append(input)
        setpoints.append(setpoint)

        # Print the received packet
        print("Received packet:", packet)

# Create a new thread for receiving the data
times = []
inputs = []
setpoints = []
thread = threading.Thread(target=receive_data)
thread.start()

# Call the animation function repeatedly to create a live plot
ani = animation.FuncAnimation(fig, animate, interval=1000)
plt.show()
