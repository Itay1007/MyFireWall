import sys


def show_fw_state():
    accepted_packets_counter_attr_fd = open(
        "/sys/class/fw_sysfs_class/fw_sysfs_device/fw_sysfs_att_dropped_packets_counter", "r")
    accepted_packets_counter = int(accepted_packets_counter_attr_fd.read())
    accepted_packets_counter_attr_fd.close()

    dropped_packets_counter_attr_fd = open(
        "/sys/class/fw_sysfs_class/fw_sysfs_device/fw_sysfs_att_accepted_packets_counter", "r")

    dropped_packets_counter = int(dropped_packets_counter_attr_fd.read())
    dropped_packets_counter_attr_fd.close()

    total_packets_counter = accepted_packets_counter + dropped_packets_counter

    print("Firewall Packets Summary:")
    print(f"Number of accepted packets: {accepted_packets_counter}")
    print(f"Number of dropped packets: {dropped_packets_counter}")
    print(f"Total number of packets: {total_packets_counter}")


def reset_fw_state():
    with open("/sys/class/fw_sysfs_class/fw_sysfs_device/fw_sysfs_att_dropped_packets_counter",
              "w") as accepted_packets_counter_attr_fd:
        accepted_packets_counter_attr_fd.write("")

    with open("/sys/class/fw_sysfs_class/fw_sysfs_device/fw_sysfs_att_accepted_packets_counter",
              "w") as dropped_packets_counter_attr_fd:
        dropped_packets_counter_attr_fd.write("")


def validate_user_input(argc, argv):
    if argc == 1:
        return
    elif argc > 2:
        print("Usage: ./user_space_program [0]\nDo not add one optional reset parameter to the program")
        exit(0)

    try:
        reset_option = int(argv[1])
        if reset_option != 0:
            print("Optional parameter should be set to '0'")
            exit(0)
    except TypeError as e:
        print("The optional argument should be set to '0'")
        exit(0)


def main(argc, argv):
    print("In main")
    validate_user_input(argc, argv)
    print("after main validation")
    perform_reset = (argc == 2)
    if perform_reset:
        reset_fw_state()
    else:
        show_fw_state()


if __name__ == "__main__":
    main(len(sys.argv), sys.argv)
