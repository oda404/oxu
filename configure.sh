sudo apt update && 
sudo apt install -y   \
    libsdl2-dev       \
    libsdl2-image-dev \
    libsdl2-mixer-dev \
    libsdl2-ttf-dev   \
    build-essential   \
    cmake

# create oXu dir the user's .config
user_config_oXu_dir=/home/$USER/.config/oXu

mkdir build $user_config_oXu_dir $user_config_oXu_dir/songs $user_config_oXu_dir/skins
cp -r res $user_config_oXu_dir 

# make and install spdlog
cd vendor/spdlog
mkdir build
cd build
cmake ..
sudo make install -j4

echo Done!