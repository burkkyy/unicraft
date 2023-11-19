FROM debian:bullseye

# Update system
RUN apt update && apt install -y

# Install build dependencies
RUN yes | apt install \
    build-essential \
    git \
    cmake \
    wget

# Install glfw
RUN yes | apt install xorg-dev
RUN git clone https://github.com/glfw/glfw.git
RUN cd glfw && \
    mkdir build && \
    cd build && \
    cmake -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF .. && \
    make && \
    make install
RUN mv /glfw/build/src/libglfw3.a /usr/local/lib/

# Install build dependencies
RUN yes | apt install \
#    vulkan-validationlayers-dev \
    spirv-tools \
    libglm-dev 
#    libxxf86vm-dev \
#    libxi-dev

# Install vulkan sdk
RUN wget https://sdk.lunarg.com/sdk/download/1.3.268.0/linux/vulkan-sdk.tar.gz
RUN tar -xvf vulkan-sdk.tar.gz
RUN mv 1.3.268.0/x86_64/lib/* /usr/local/lib/
RUN mv 1.3.268.0/x86_64/include/* /usr/local/include/
RUN mv 1.3.268.0/x86_64/bin/glslc /usr/local/bin/glslc

VOLUME /root/env
WORKDIR /root/env
