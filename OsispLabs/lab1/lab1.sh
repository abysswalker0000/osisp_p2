#!/bin/bash

generate_random() {
    local max=$1
    seed=${seed:-$(date +%s)}
    seed=$(( (seed * 1103515245 + 12345) % 2**31 ))
    echo $(( seed % max ))
}

clear_screen() {
    printf "\033[H\033[2J"
}

set_cursor_position() {
    local x=$1
    local y=$2
    printf "\033[${y};${x}H"
}

get_big_digit() {
    case $1 in
        0) echo -e " __ \n|  |\n|__|" ;;
        1) echo -e "    \n   |\n   |" ;;
        2) echo -e " __ \n __|\n|__ " ;;
        3) echo -e " __ \n __|\n __|" ;;
        4) echo -e "    \n|__|\n   |" ;;
        5) echo -e " __ \n|__ \n __|" ;;
        6) echo -e " __ \n|__ \n|__|" ;;
        7) echo -e " __ \n   |\n   |" ;;
        8) echo -e " __ \n|__|\n|__|" ;;
        9) echo -e " __ \n|__|\n __|" ;;
        :) echo -e "    \n  : \n    " ;;
    esac
}

x=0
y=0
last_position_change_time=0

while true; do
    clear_screen
    width=$(tput cols)
    height=$(tput lines)
    current_time=$(date +%s)

    if (( current_time - last_position_change_time >= 5 )); then
        x=$(generate_random $((width - 40))) 
        y=$(generate_random $((height - 5)))
        last_position_change_time=$current_time
    fi

    time_str=$(date +"%H:%M:%S")
    big_time_lines=("" "" "")

    for (( i=0; i<${#time_str}; i++ )); do
        digit=${time_str:i:1}
        digit_art=$(get_big_digit "$digit")
        line1=$(echo "$digit_art" | sed -n '1p')
        line2=$(echo "$digit_art" | sed -n '2p')
        line3=$(echo "$digit_art" | sed -n '3p')

        if (( i > 0 )); then
            big_time_lines[0]+="  "
            big_time_lines[1]+="  "
            big_time_lines[2]+="  "
        fi

        big_time_lines[0]+="$line1"
        big_time_lines[1]+="$line2"
        big_time_lines[2]+="$line3"
    done

    for (( i=0; i<3; i++ )); do
        set_cursor_position $((x + 1)) $((y + 1 + i))
        printf "%s" "${big_time_lines[i]}"
    done

    sleep 1
done