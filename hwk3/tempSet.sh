#set high or low triggers for 2 i2c devices
if [ $# -lt 4 ]; then
    echo "Usage: $0 i2cBus dir ic2Addr1 ic2Addr2 temp"
    exit 0
fi

i2cBus=$1
dir=$2
i2cAddr1=$3
i2cAddr2=$4
temp=$5
`i2cset -y ${i2cBus} ${i2cAddr1} ${dir} ${temp} `
`i2cset -y ${i2cBus} ${i2cAddr2} ${dir} ${temp} `
