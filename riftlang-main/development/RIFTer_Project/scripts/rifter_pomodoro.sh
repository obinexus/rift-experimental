#!/bin/bash
# rifter_pomodoro.sh - RIFTer Two-Track Pomodoro Timer
# Following The RIFTer's Way: "Pomodoro by pomodoro. A goal. A breath. A push. A rest."

# ANSI color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

# RIFTer Pomodoro settings (45-minute sessions)
WORK_MINUTES=45
BREAK_MINUTES=15
CURRENT_TRACK="foundation"

# Function to display RIFTer timer
display_rifter_timer() {
    local minutes=$1
    local seconds=$2
    local type=$3
    local track=$4
    
    clear
    echo -e "${CYAN}"
    echo " ██████╗ ██╗███████╗████████╗███████╗██████╗ "
    echo " ██╔══██╗██║██╔════╝╚══██╔══╝██╔════╝██╔══██╗"
    echo " ██████╔╝██║█████╗     ██║   █████╗  ██████╔╝"
    echo " ██╔══██╗██║██╔══╝     ██║   ██╔══╝  ██╔══██╗"
    echo " ██║  ██║██║██║        ██║   ███████╗██║  ██║"
    echo " ╚═╝  ╚═╝╚═╝╚═╝        ╚═╝   ╚══════╝╚═╝  ╚═╝"
    echo -e "${NC}"
    
    echo ""
    echo -e "${MAGENTA}RIFTer Development Pomodoro${NC}"
    echo -e "${BLUE}Computing from the Heart${NC}"
    echo ""
    
    if [[ "$type" == "work" ]]; then
        echo -e "${GREEN}🧠 WORKING - Track: ${track}${NC}"
        echo -e "${YELLOW}\"Each token is a breath. Each breath is a truth.\"${NC}"
    else
        echo -e "${CYAN}🌱 BREATHING - Rest Period${NC}"
        echo -e "${YELLOW}\"We do not optimise ourselves away.\"${NC}"
    fi
    
    echo ""
    echo -e "${CYAN}⏱️ ${minutes}:$(printf "%02d" ${seconds})${NC}"
    echo ""
    
    if [[ "$type" == "work" ]]; then
        echo -e "${GREEN}Focus Areas:${NC}"
        if [[ "$track" == "foundation" ]]; then
            echo "• Semantic token architecture research"
            echo "• Single-pass compilation principles"
            echo "• Memory governance patterns"
        else
            echo "• RIFTlang implementation"
            echo "• Token structure coding"
            echo "• Policy enforcement development"
        fi
    fi
}

# Run Pomodoro timer
run_rifter_timer() {
    local duration_minutes=$1
    local type=$2
    
    local end_time=$(($(date +%s) + duration_minutes * 60))
    
    while [[ $(date +%s) -lt $end_time ]]; do
        local remaining_seconds=$((end_time - $(date +%s)))
        local minutes=$((remaining_seconds / 60))
        local seconds=$((remaining_seconds % 60))
        
        display_rifter_timer $minutes $seconds $type $CURRENT_TRACK
        sleep 1
    done
    
    # Completion notification
    echo -e "${GREEN}✓ Session Complete!${NC}"
    if [[ "$type" == "work" ]]; then
        echo -e "${YELLOW}Time for a breath. Take your rest.${NC}"
    else
        echo -e "${YELLOW}Ready to return to focused work.${NC}"
    fi
}

# Parse command line arguments
if [[ "$1" == "--track" ]]; then
    CURRENT_TRACK="$2"
fi

echo -e "${MAGENTA}Starting RIFTer Pomodoro for ${CURRENT_TRACK} track${NC}"
echo -e "${CYAN}Press Ctrl+C to exit${NC}"
echo ""

while true; do
    run_rifter_timer $WORK_MINUTES "work"
    read -p "Press Enter to start break..."
    run_rifter_timer $BREAK_MINUTES "break"
    read -p "Press Enter to start next work session..."
done
