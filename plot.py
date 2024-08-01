import serial
import argparse

def main():
    #read sieral port name from args
    parser = argparse.ArgumentParser()
    parser.add_argument("serial_port", help="Serial port to connect to")
    port = parser.parse_args().serial_port

    with serial.Serial(port, 115200, timeout=1) as ser:
        while True:
            for line in ser.readlines():
                print(line)


if __name__ == '__main__':
    main()
