import socket

UDP_IP = "255.255.255.255"
UDP_PORT = 40292
MESSAGE = "GFE_reflowOven_uThrBro?!"

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP) # UDP
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP, UDP_PORT))

while True:
    data, address = sock.recvfrom(4096)
    print("Server received: ", data.decode('utf-8'), "from", address ,"\n\n")
    


sock.close()
