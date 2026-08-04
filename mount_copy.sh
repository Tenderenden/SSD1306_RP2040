#!/bin/bash

# --- Configuration ---
DEVICE="RPI-RP2"           # The partition you want to mount
MOUNT_POINT="/mnt/rp2040"   # Where it will be mounted
SOURCE_FILE="/home/ttendera/Documents/projects/SSD1306_RP2040/build/blink.uf2"
DEST_DIR="$MOUNT_POINT"

# --- Script Logic ---

# 1. Create mount point if it doesn't exist
if [ ! -d "$MOUNT_POINT" ]; then
    echo "Creating mount point..."
    sudo mkdir -p "$MOUNT_POINT"
fi

# 2. Mount the device
echo "Mounting $DEVICE to $MOUNT_POINT..."
if sudo mount -L "$DEVICE" "$MOUNT_POINT"; then
    echo "Mount successful."
else
    echo "Error: Failed to mount $DEVICE."
    exit 1
fi

# 3. Create destination directory if needed
sudo mkdir -p "$DEST_DIR"

# 4. Copy the file
echo "Copying $SOURCE_FILE to $DEST_DIR..."
if sudo cp "$SOURCE_FILE" "$DEST_DIR"; then
    echo "File copied successfully!"
else
    echo "Error: File copy failed."
fi

# 5. Optional: Unmount when done
# echo "Unmounting..."
sudo umount "$MOUNT_POINT"

echo "Done."