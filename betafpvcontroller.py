import usb.core
import usb.backend

VENDOR_ID = 0x0483
PRODUCT_ID = 0x5750

device = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)

# Ensure the device was found
if device is None:
    raise ValueError("Device not found")

# Set the active configuration if not already set
device.set_configuration()

# Find the interface (usually 0) and endpoint for reading
cfg = device.get_active_configuration()
intf = cfg[(0, 0)]  # Interface (0,0) is usually the default

# Find the IN endpoint (usually with address 0x81 for bulk read)
endpoint = None
for ep in intf:
    if usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_IN:
        endpoint = ep
        break

if endpoint is None:
    raise ValueError("No IN endpoint found")

while True:
    data = device.read(endpoint.bEndpointAddress, 16, timeout=1000)
    print(data)
    