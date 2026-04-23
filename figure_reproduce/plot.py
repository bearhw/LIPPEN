import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Patch

# =========================================================
#                     STYLE CONTROLS
# =========================================================
FIG_WIDTH, FIG_HEIGHT = 20, 5.0 # Increased height slightly for better label spacing
DPI = 300
YMAX_LEFT  = 450.0
YMAX_RIGHT = 140.0

LABEL_FONTSIZE = 17
XTICK_FONTSIZE = 13
YTICK_FONTSIZE = 18 # <--- INCREASED THIS for larger Y-axis numbers
LEGEND_FONTSIZE = 17
TITLE_FONTSIZE = 18

BAR_WIDTH = 0.08

# =========================================================
#  COLORS
# =========================================================
C_PRINCE_R = "#76b7ff" # Light Blue
C_QARMA_R  = "#f5a623" # Yellow/Gold
C_PRINCE_B = "#0d47a1" # Dark Blue
C_QARMA_B  = "#ef6c00" # Orange
C_APPLE    = "#000000" # Black


# =========================================================
# =========================================================
# =========================================================
# =========================================================
# =========================================================
# =========================================================
# =========================================================
# =========================================================
# =========================================================

# =========================================================
# APPLE RAW DATA
# =========================================================
# get it from loop_l.c
#loop_l_nopac
APPLE_LOOP_L_NOPAC = 
#loop_l_pac
apple_loop_l_pac = 
apple_loop_l_pac_overhead = ((apple_loop_l_pac - APPLE_LOOP_L_NOPAC) / APPLE_LOOP_L_NOPAC) * 100

# get it from loop_s.c
#loop_s_nopac
APPLE_LOOP_S_NOPAC = 
#loop_s_pac
apple_loop_s_pac = 
apple_loop_s_pac_overhead = ((apple_loop_s_pac - APPLE_LOOP_S_NOPAC) / APPLE_LOOP_S_NOPAC) * 100

# get it from loop_nested.c
# loop_nested_nopac
APPLE_LOOP_NESTED_NOPAC = 
# loop_nested_pac
apple_loop_nested_pac =
apple_loop_nested_pac_overhead = ((apple_loop_nested_pac - APPLE_LOOP_NESTED_NOPAC) / APPLE_LOOP_NESTED_NOPAC) * 100

# get it from deep_recursive_return.c
# deep_recursive_return_nopac
APPLE_DEEP_RECURSIVE_RETURN_NOPAC =
# deep_recursive_return_pac
apple_deep_recursive_return_pac = 
apple_deep_recursive_return_pac_overhead = ((apple_deep_recursive_return_pac - APPLE_DEEP_RECURSIVE_RETURN_NOPAC) / APPLE_DEEP_RECURSIVE_RETURN_NOPAC) * 100

# get it from deep_recursive_entry.c
# deep_recursive_entry_nopac
APPLE_DEEP_RECURSIVE_ENTRY_NOPAC =
# deep_recursive_entry_pac
apple_deep_recursive_entry_pac =
apple_deep_recursive_entry_pac_overhead = ((apple_deep_recursive_entry_pac - APPLE_DEEP_RECURSIVE_ENTRY_NOPAC) / APPLE_DEEP_RECURSIVE_ENTRY_NOPAC) * 100

# get it from data_loop.c
# put the number you see in front of LDR=
APPLE_DATA_LOOP_NOPAC = 
# put the number you see in front of LDRAA=
apple_data_loop_pac_fused = 
# put the number you see in front of LDR+AUTDA=
apple_data_loop_pac_split = 
# put the number you see in front of LDR+AUTIA(c)=
apple_data_loop_pac_split_ia = 
apple_data_loop_pac_fused_overhead = ((apple_data_loop_pac_fused - APPLE_DATA_LOOP_NOPAC) / APPLE_DATA_LOOP_NOPAC) * 100
apple_data_loop_pac_split_overhead = ((apple_data_loop_pac_split - APPLE_DATA_LOOP_NOPAC) / APPLE_DATA_LOOP_NOPAC) * 100
apple_data_loop_pac_split_ia_overhead = ((apple_data_loop_pac_split_ia - APPLE_DATA_LOOP_NOPAC) / APPLE_DATA_LOOP_NOPAC) * 100


# get it from data_unrolled32.c
# put the number you see in front of LDR=
APPLE_DATA_UNRROLLED32_NOPAC =
# put the number you see in front of LDRAA=
apple_data_unrolled32_pac_fused = 
# put the number you see in front of LDR+AUTDA=
apple_data_unrolled32_pac_split = 
# put the number you see in front of LDR+AUTIA(c)=
apple_data_unrolled32_pac_split_const = 
# put the number you see in front of LDR+AUTIA(reload)=
apple_data_unrolled32_pac_split_reload = 
apple_data_unrolled32_pac_fused_overhead = ((apple_data_unrolled32_pac_fused - APPLE_DATA_UNRROLLED32_NOPAC) / APPLE_DATA_UNRROLLED32_NOPAC) * 100
apple_data_unrolled32_pac_split_overhead = ((apple_data_unrolled32_pac_split - APPLE_DATA_UNRROLLED32_NOPAC) / APPLE_DATA_UNRROLLED32_NOPAC) * 100
apple_data_unrolled32_pac_split_const_overhead = ((apple_data_unrolled32_pac_split_const - APPLE_DATA_UNRROLLED32_NOPAC) / APPLE_DATA_UNRROLLED32_NOPAC) * 100
apple_data_unrolled32_pac_split_reload_overhead = ((apple_data_unrolled32_pac_split_reload - APPLE_DATA_UNRROLLED32_NOPAC) / APPLE_DATA_UNRROLLED32_NOPAC) * 100

# =========================================================
# ROCKET PAC/QARMA RAW DATA
# =========================================================
# get it from loop_l.c
#loop_l_nopac.linux.riscv --> you should name it like that while you are generating it
ROCKET_LOOP_L_NOPAC = 
#loop_l.linux.riscv
rocket_loop_l_pac = 
rocket_loop_l_pac_overhead = ((rocket_loop_l_pac - ROCKET_LOOP_L_NOPAC) / ROCKET_LOOP_L_NOPAC) * 100

# get it from loop_s.c
#loop_s_nopac.linux.riscv --> you should name it like that while you are generating it
ROCKET_LOOP_S_NOPAC = 
#loop_s.linux.riscv
rocket_loop_s_pac = 
rocket_loop_s_pac_overhead = ((rocket_loop_s_pac - ROCKET_LOOP_S_NOPAC) / ROCKET_LOOP_S_NOPAC) * 100

# get it from loop_nested.c
#loop_nested_nopac.linux.riscv --> you should name it like that while you are generating it
ROCKET_LOOP_NESTED_NOPAC = 
#loop_nested.linux.riscv
rocket_loop_nested_pac =
rocket_loop_nested_pac_overhead = ((rocket_loop_nested_pac - ROCKET_LOOP_NESTED_NOPAC) / ROCKET_LOOP_NESTED_NOPAC) * 100

# get it from deep_recursive_return.c
#deep_recursive_return_nopac.linux.riscv --> you should name it like that while you are generating it
ROCKET_DEEP_RECURSIVE_RETURN_NOPAC =
#deep_recursive_return.linux.riscv --> you should name it like that while you are generating it
rocket_deep_recursive_return_pac = 
rocket_deep_recursive_return_pac_overhead = ((rocket_deep_recursive_return_pac - ROCKET_DEEP_RECURSIVE_RETURN_NOPAC) / ROCKET_DEEP_RECURSIVE_RETURN_NOPAC) * 100

# get it from deep_recursive_entry.c
#deep_recursive_entry_nopac.linux.riscv --> you should name it like that while you are generating it
ROCKET_DEEP_RECURSIVE_ENTRY_NOPAC =
#deep_recursive_entry.linux.riscv --> you should name it like that while you are generating it
rocket_deep_recursive_entry_pac =
rocket_deep_recursive_entry_pac_overhead = ((rocket_deep_recursive_entry_pac - ROCKET_DEEP_RECURSIVE_ENTRY_NOPAC) / ROCKET_DEEP_RECURSIVE_ENTRY_NOPAC) * 100

# get it from data_loop.c
# put the number you see in front of nopac=
ROCKET_DATA_LOOP_NOPAC = 
# put the number you see in front of pac=
rocket_data_loop_pac_split = 
# put the number you see in front of pac_ia=
rocket_data_loop_pac_split_ia = 
rocket_data_loop_pac_split_overhead = ((rocket_data_loop_pac_split - ROCKET_DATA_LOOP_NOPAC) / ROCKET_DATA_LOOP_NOPAC) * 100
rocket_data_loop_pac_split_ia_overhead = ((rocket_data_loop_pac_split_ia - ROCKET_DATA_LOOP_NOPAC) / ROCKET_DATA_LOOP_NOPAC) * 100

# get it from data_unrolled32.c
# put the number you see in front of nopac=
ROCKET_DATA_UNRROLLED32_NOPAC =
# put the number you see in front of pac=
rocket_data_unrolled32_pac_split = 
# put the number you see in front of pac_ia=
rocket_data_unrolled32_pac_split_const = 
# put the number you see in front of pac_ia_reload=
rocket_data_unrolled32_pac_split_reload = 
rocket_data_unrolled32_pac_split_overhead = ((rocket_data_unrolled32_pac_split - ROCKET_DATA_UNRROLLED32_NOPAC) / ROCKET_DATA_UNRROLLED32_NOPAC) * 100
rocket_data_unrolled32_pac_split_const_overhead = ((rocket_data_unrolled32_pac_split_const - ROCKET_DATA_UNRROLLED32_NOPAC) / ROCKET_DATA_UNRROLLED32_NOPAC) * 100
rocket_data_unrolled32_pac_split_reload_overhead = ((rocket_data_unrolled32_pac_split_reload - ROCKET_DATA_UNRROLLED32_NOPAC) / ROCKET_DATA_UNRROLLED32_NOPAC) * 100

# =========================================================
# ROCKET LIPPEN/PRINCE RAW DATA
# =========================================================
# get it from loop_l.c
#loop_l_nopac.linux.riscv --> you should name it like that while you are generating it
ROCKET_LOOP_L_NOLIPPEN = 
#loop_l.linux.riscv
rocket_loop_l_lippen = 
rocket_loop_l_lippen_overhead = ((rocket_loop_l_lippen - ROCKET_LOOP_L_NOLIPPEN) / ROCKET_LOOP_L_NOLIPPEN) * 100

# get it from loop_s.c
#loop_s_nopac.linux.riscv --> you should name it like that while you are generating it
ROCKET_LOOP_S_NOLIPPEN = 
#loop_s.linux.riscv
rocket_loop_s_lippen = 
rocket_loop_s_lippen_overhead = ((rocket_loop_s_lippen - ROCKET_LOOP_S_NOLIPPEN) / ROCKET_LOOP_S_NOLIPPEN) * 100

# get it from loop_nested.c
#loop_nested_nopac.linux.riscv --> you should name it like that while you are generating it
ROCKET_LOOP_NESTED_NOLIPPEN = 
#loop_nested.linux.riscv
rocket_loop_nested_lippen =
rocket_loop_nested_lippen_overhead = ((rocket_loop_nested_lippen - ROCKET_LOOP_NESTED_NOLIPPEN) / ROCKET_LOOP_NESTED_NOLIPPEN) * 100

# get it from deep_recursive_return.c
#deep_recursive_return_nopac.linux.riscv --> you should name it like that while you are generating it
ROCKET_DEEP_RECURSIVE_RETURN_NOLIPPEN =
#deep_recursive_return.linux.riscv --> you should name it like that while you are generating it
rocket_deep_recursive_return_lippen = 
rocket_deep_recursive_return_lippen_overhead = ((rocket_deep_recursive_return_lippen - ROCKET_DEEP_RECURSIVE_RETURN_NOLIPPEN) / ROCKET_DEEP_RECURSIVE_RETURN_NOLIPPEN) * 100

# get it from deep_recursive_entry.c
#deep_recursive_entry_nopac.linux.riscv --> you should name it like that while you are generating it
ROCKET_DEEP_RECURSIVE_ENTRY_NOLIPPEN =
#deep_recursive_entry.linux.riscv --> you should name it like that while you are generating it
rocket_deep_recursive_entry_lippen =
rocket_deep_recursive_entry_lippen_overhead = ((rocket_deep_recursive_entry_lippen - ROCKET_DEEP_RECURSIVE_ENTRY_NOLIPPEN) / ROCKET_DEEP_RECURSIVE_ENTRY_NOLIPPEN) * 100

# get it from data_loop.c
# put the number you see in front of nopac=
ROCKET_DATA_LOOP_NOLIPPEN = 
# put the number you see in front of pac=
rocket_data_loop_lippen_split = 
# put the number you see in front of pac_ia=
rocket_data_loop_lippen_split_ia = 
rocket_data_loop_lippen_split_overhead = ((rocket_data_loop_lippen_split - ROCKET_DATA_LOOP_NOLIPPEN) / ROCKET_DATA_LOOP_NOLIPPEN) * 100
rocket_data_loop_lippen_split_ia_overhead = ((rocket_data_loop_lippen_split_ia - ROCKET_DATA_LOOP_NOLIPPEN) / ROCKET_DATA_LOOP_NOLIPPEN) * 100

# get it from data_unrolled32.c
# put the number you see in front of nopac=
ROCKET_DATA_UNRROLLED32_NOLIPPEN =
# put the number you see in front of pac=
rocket_data_unrolled32_lippen_split = 
# put the number you see in front of pac_ia=
rocket_data_unrolled32_lippen_split_const = 
# put the number you see in front of pac_ia_reload=
rocket_data_unrolled32_lippen_split_reload = 
rocket_data_unrolled32_lippen_split_overhead = ((rocket_data_unrolled32_lippen_split - ROCKET_DATA_UNRROLLED32_NOLIPPEN) / ROCKET_DATA_UNRROLLED32_NOLIPPEN) * 100
rocket_data_unrolled32_lippen_split_const_overhead = ((rocket_data_unrolled32_lippen_split_const - ROCKET_DATA_UNRROLLED32_NOLIPPEN) / ROCKET_DATA_UNRROLLED32_NOLIPPEN) * 100
rocket_data_unrolled32_lippen_split_reload_overhead = ((rocket_data_unrolled32_lippen_split_reload - ROCKET_DATA_UNRROLLED32_NOLIPPEN) / ROCKET_DATA_UNRROLLED32_NOLIPPEN) * 100


# =========================================================
# BOOM PAC/QARMA RAW DATA
# =========================================================
# get it from loop_l.c
#loop_l_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_LOOP_L_NOPAC = 
#loop_l.linux.riscv
boom_loop_l_pac = 
boom_loop_l_pac_overhead = ((boom_loop_l_pac - BOOM_LOOP_L_NOPAC) / BOOM_LOOP_L_NOPAC) * 100

# get it from loop_s.c
#loop_s_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_LOOP_S_NOPAC = 
#loop_s.linux.riscv
boom_loop_s_pac = 
boom_loop_s_pac_overhead = ((boom_loop_s_pac - BOOM_LOOP_S_NOPAC) / BOOM_LOOP_S_NOPAC) * 100

# get it from loop_nested.c
#loop_nested_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_LOOP_NESTED_NOPAC = 
#loop_nested.linux.riscv
boom_loop_nested_pac =
boom_loop_nested_pac_overhead = ((boom_loop_nested_pac - BOOM_LOOP_NESTED_NOPAC) / BOOM_LOOP_NESTED_NOPAC) * 100

# get it from deep_recursive_return.c
#deep_recursive_return_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_DEEP_RECURSIVE_RETURN_NOPAC =
#deep_recursive_return.linux.riscv --> you should name it like that while you are generating it
boom_deep_recursive_return_pac = 
boom_deep_recursive_return_pac_overhead = ((boom_deep_recursive_return_pac - BOOM_DEEP_RECURSIVE_RETURN_NOPAC) / BOOM_DEEP_RECURSIVE_RETURN_NOPAC) * 100

# get it from deep_recursive_entry.c
#deep_recursive_entry_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_DEEP_RECURSIVE_ENTRY_NOPAC =
#deep_recursive_entry.linux.riscv --> you should name it like that while you are generating it
boom_deep_recursive_entry_pac =
boom_deep_recursive_entry_pac_overhead = ((boom_deep_recursive_entry_pac - BOOM_DEEP_RECURSIVE_ENTRY_NOPAC) / BOOM_DEEP_RECURSIVE_ENTRY_NOPAC) * 100

# get it from data_loop.c
# put the number you see in front of nopac=
BOOM_DATA_LOOP_NOPAC = 
# put the number you see in front of pac=
boom_data_loop_pac_split = 
# put the number you see in front of pac_ia=
boom_data_loop_pac_split_ia = 
boom_data_loop_pac_split_overhead = ((boom_data_loop_pac_split - BOOM_DATA_LOOP_NOPAC) / BOOM_DATA_LOOP_NOPAC) * 100
boom_data_loop_pac_split_ia_overhead = ((boom_data_loop_pac_split_ia - BOOM_DATA_LOOP_NOPAC) / BOOM_DATA_LOOP_NOPAC) * 100

# get it from data_unrolled32.c
# put the number you see in front of nopac=
BOOM_DATA_UNRROLLED32_NOPAC =
# put the number you see in front of pac=
boom_data_unrolled32_pac_split = 
# put the number you see in front of pac_ia=
boom_data_unrolled32_pac_split_const = 
# put the number you see in front of pac_ia_reload=
boom_data_unrolled32_pac_split_reload = 
boom_data_unrolled32_pac_split_overhead = ((boom_data_unrolled32_pac_split - BOOM_DATA_UNRROLLED32_NOPAC) / BOOM_DATA_UNRROLLED32_NOPAC) * 100
boom_data_unrolled32_pac_split_const_overhead = ((boom_data_unrolled32_pac_split_const - BOOM_DATA_UNRROLLED32_NOPAC) / BOOM_DATA_UNRROLLED32_NOPAC) * 100
boom_data_unrolled32_pac_split_reload_overhead = ((boom_data_unrolled32_pac_split_reload - BOOM_DATA_UNRROLLED32_NOPAC) / BOOM_DATA_UNRROLLED32_NOPAC) * 100



# =========================================================
# BOOM LIPPEN/PRINCE RAW DATA
# =========================================================
# get it from loop_l.c
#loop_l_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_LOOP_L_NOLIPPEN = 
#loop_l.linux.riscv
boom_loop_l_lippen = 
boom_loop_l_lippen_overhead = ((boom_loop_l_lippen - BOOM_LOOP_L_NOLIPPEN) / BOOM_LOOP_L_NOLIPPEN) * 100

# get it from loop_s.c
#loop_s_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_LOOP_S_NOLIPPEN = 
#loop_s.linux.riscv
boom_loop_s_lippen = 
boom_loop_s_lippen_overhead = ((boom_loop_s_lippen - BOOM_LOOP_S_NOLIPPEN) / BOOM_LOOP_S_NOLIPPEN) * 100

# get it from loop_nested.c
#loop_nested_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_LOOP_NESTED_NOLIPPEN = 
#loop_nested.linux.riscv
boom_loop_nested_lippen =
boom_loop_nested_lippen_overhead = ((boom_loop_nested_lippen - BOOM_LOOP_NESTED_NOLIPPEN) / BOOM_LOOP_NESTED_NOLIPPEN) * 100

# get it from deep_recursive_return.c
#deep_recursive_return_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_DEEP_RECURSIVE_RETURN_NOLIPPEN =
#deep_recursive_return.linux.riscv --> you should name it like that while you are generating it
boom_deep_recursive_return_lippen = 
boom_deep_recursive_return_lippen_overhead = ((boom_deep_recursive_return_lippen - BOOM_DEEP_RECURSIVE_RETURN_NOLIPPEN) / BOOM_DEEP_RECURSIVE_RETURN_NOLIPPEN) * 100

# get it from deep_recursive_entry.c
#deep_recursive_entry_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_DEEP_RECURSIVE_ENTRY_NOLIPPEN =
#deep_recursive_entry.linux.riscv --> you should name it like that while you are generating it
boom_deep_recursive_entry_lippen =
boom_deep_recursive_entry_lippen_overhead = ((boom_deep_recursive_entry_lippen - BOOM_DEEP_RECURSIVE_ENTRY_NOLIPPEN) / BOOM_DEEP_RECURSIVE_ENTRY_NOLIPPEN) * 100

# get it from data_loop.c
# put the number you see in front of nopac=
BOOM_DATA_LOOP_NOLIPPEN = 
# put the number you see in front of pac=
boom_data_loop_lippen_split = 
# put the number you see in front of pac_ia=
boom_data_loop_lippen_split_ia = 
boom_data_loop_lippen_split_overhead = ((boom_data_loop_lippen_split - BOOM_DATA_LOOP_NOLIPPEN) / BOOM_DATA_LOOP_NOLIPPEN) * 100
boom_data_loop_lippen_split_ia_overhead = ((boom_data_loop_lippen_split_ia - BOOM_DATA_LOOP_NOLIPPEN) / BOOM_DATA_LOOP_NOLIPPEN) * 100

# get it from data_unrolled32.c
# put the number you see in front of nopac=
BOOM_DATA_UNRROLLED32_NOLIPPEN =
# put the number you see in front of pac=
boom_data_unrolled32_lippen_split = 
# put the number you see in front of pac_ia=
boom_data_unrolled32_lippen_split_const = 
# put the number you see in front of pac_ia_reload=
boom_data_unrolled32_lippen_split_reload = 
boom_data_unrolled32_lippen_split_overhead = ((boom_data_unrolled32_lippen_split - BOOM_DATA_UNRROLLED32_NOLIPPEN) / BOOM_DATA_UNRROLLED32_NOLIPPEN) * 100
boom_data_unrolled32_lippen_split_const_overhead = ((boom_data_unrolled32_lippen_split_const - BOOM_DATA_UNRROLLED32_NOLIPPEN) / BOOM_DATA_UNRROLLED32_NOLIPPEN) * 100
boom_data_unrolled32_lippen_split_reload_overhead = ((boom_data_unrolled32_lippen_split_reload - BOOM_DATA_UNRROLLED32_NOLIPPEN) / BOOM_DATA_UNRROLLED32_NOLIPPEN) * 100



# =========================================================
# BOOM CONFIG1 (NO RAS) LIPPEN/PRINCE RAW DATA
# =========================================================
# get it from loop_l.c
#loop_l_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_custom_LOOP_L_NOLIPPEN = 
#loop_l.linux.riscv
boom_custom_loop_l_lippen = 
boom_custom_loop_l_lippen_overhead = ((boom_custom_loop_l_lippen - BOOM_custom_LOOP_L_NOLIPPEN) / BOOM_custom_LOOP_L_NOLIPPEN) * 100

# get it from loop_s.c
#loop_s_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_custom_LOOP_S_NOLIPPEN = 
#loop_s.linux.riscv
boom_custom_loop_s_lippen = 
boom_custom_loop_s_lippen_overhead = ((boom_custom_loop_s_lippen - BOOM_custom_LOOP_S_NOLIPPEN) / BOOM_custom_LOOP_S_NOLIPPEN) * 100

# get it from loop_nested.c
#loop_nested_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_custom_LOOP_NESTED_NOLIPPEN = 
#loop_nested.linux.riscv
boom_custom_loop_nested_lippen =
boom_custom_loop_nested_lippen_overhead = ((boom_custom_loop_nested_lippen - BOOM_custom_LOOP_NESTED_NOLIPPEN) / BOOM_custom_LOOP_NESTED_NOLIPPEN) * 100

# get it from deep_recursive_return.c
#deep_recursive_return_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_custom_DEEP_RECURSIVE_RETURN_NOLIPPEN =
#deep_recursive_return.linux.riscv --> you should name it like that while you are generating it
boom_custom_deep_recursive_return_lippen = 
boom_custom_deep_recursive_return_lippen_overhead = ((boom_custom_deep_recursive_return_lippen - BOOM_custom_DEEP_RECURSIVE_RETURN_NOLIPPEN) / BOOM_custom_DEEP_RECURSIVE_RETURN_NOLIPPEN) * 100

# get it from deep_recursive_entry.c
#deep_recursive_entry_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_custom_DEEP_RECURSIVE_ENTRY_NOLIPPEN =
#deep_recursive_entry.linux.riscv --> you should name it like that while you are generating it
boom_custom_deep_recursive_entry_lippen =
boom_custom_deep_recursive_entry_lippen_overhead = ((boom_custom_deep_recursive_entry_lippen - BOOM_custom_DEEP_RECURSIVE_ENTRY_NOLIPPEN) / BOOM_custom_DEEP_RECURSIVE_ENTRY_NOLIPPEN) * 100



# =========================================================
# BOOM CONFIG2 (NO RAS, NO BTB) LIPPEN/PRINCE RAW DATA
# =========================================================
# get it from loop_l.c
#loop_l_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_custom2_LOOP_L_NOLIPPEN = 
#loop_l.linux.riscv
boom_custom2_loop_l_lippen = 
boom_custom2_loop_l_lippen_overhead = ((boom_custom2_loop_l_lippen - BOOM_custom2_LOOP_L_NOLIPPEN) / BOOM_custom2_LOOP_L_NOLIPPEN) * 100

# get it from loop_s.c
#loop_s_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_custom2_LOOP_S_NOLIPPEN = 
#loop_s.linux.riscv
boom_custom2_loop_s_lippen = 
boom_custom2_loop_s_lippen_overhead = ((boom_custom2_loop_s_lippen - BOOM_custom2_LOOP_S_NOLIPPEN) / BOOM_custom2_LOOP_S_NOLIPPEN) * 100

# get it from loop_nested.c
#loop_nested_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_custom2_LOOP_NESTED_NOLIPPEN = 
#loop_nested.linux.riscv
boom_custom2_loop_nested_lippen =
boom_custom2_loop_nested_lippen_overhead = ((boom_custom2_loop_nested_lippen - BOOM_custom2_LOOP_NESTED_NOLIPPEN) / BOOM_custom2_LOOP_NESTED_NOLIPPEN) * 100

# get it from deep_recursive_return.c
#deep_recursive_return_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_custom2_DEEP_RECURSIVE_RETURN_NOLIPPEN =
#deep_recursive_return.linux.riscv --> you should name it like that while you are generating it
boom_custom2_deep_recursive_return_lippen = 
boom_custom2_deep_recursive_return_lippen_overhead = ((boom_custom2_deep_recursive_return_lippen - BOOM_custom2_DEEP_RECURSIVE_RETURN_NOLIPPEN) / BOOM_custom2_DEEP_RECURSIVE_RETURN_NOLIPPEN) * 100

# get it from deep_recursive_entry.c
#deep_recursive_entry_nopac.linux.riscv --> you should name it like that while you are generating it
BOOM_custom2_DEEP_RECURSIVE_ENTRY_NOLIPPEN =
#deep_recursive_entry.linux.riscv --> you should name it like that while you are generating it
boom_custom2_deep_recursive_entry_lippen =
boom_custom2_deep_recursive_entry_lippen_overhead = ((boom_custom2_deep_recursive_entry_lippen - BOOM_custom2_DEEP_RECURSIVE_ENTRY_NOLIPPEN) / BOOM_custom2_DEEP_RECURSIVE_ENTRY_NOLIPPEN) * 100


# =========================================================
# =========================================================
# =========================================================
# =========================================================
# =========================================================
# =========================================================
# =========================================================
# =========================================================
# =========================================================


# =========================================================
#  DATA
# =========================================================
left_subgroups = ["mod=0", "mod=const", "mod=non-zero-2"]

left_data_unrolled = {
    "mod=0": [(C_APPLE, "", apple_data_unrolled32_pac_split_overhead), (C_APPLE, "..", apple_data_unrolled32_pac_fused_overhead), (C_QARMA_R, "", rocket_data_unrolled32_pac_split_overhead),
              (C_PRINCE_R, "", rocket_data_unrolled32_lippen_split_overhead), (C_QARMA_B, "", boom_data_unrolled32_pac_split_overhead), (C_PRINCE_B, "", boom_data_unrolled32_lippen_split_overhead)],
    "mod=const": [(C_APPLE, "", apple_data_unrolled32_pac_split_const_overhead), (C_QARMA_R, "", rocket_data_unrolled32_pac_split_const_overhead), (C_PRINCE_R, "", rocket_data_unrolled32_lippen_split_const_overhead),
                  (C_QARMA_B, "", boom_data_unrolled32_pac_split_const_overhead), (C_PRINCE_B, "", boom_data_unrolled32_lippen_split_const_overhead)],
    "mod=non-zero-2": [(C_APPLE, "", apple_data_unrolled32_pac_split_reload_overhead), (C_QARMA_R, "", rocket_data_unrolled32_pac_split_reload_overhead), (C_PRINCE_R, "", rocket_data_unrolled32_lippen_split_reload_overhead),
                       (C_QARMA_B, "", boom_data_unrolled32_pac_split_reload_overhead), (C_PRINCE_B, "", boom_data_unrolled32_lippen_split_reload_overhead)],
}

left_data_rolled = {
    "mod=0": [(C_APPLE, "", apple_data_loop_pac_split_overhead), (C_APPLE, "..", apple_data_loop_pac_fused_overhead), (C_QARMA_R, "", rocket_data_loop_pac_split_overhead),
              (C_PRINCE_R, "", rocket_data_loop_lippen_split_overhead), (C_QARMA_B, "", boom_data_loop_pac_split_overhead), (C_PRINCE_B, "", boom_data_loop_lippen_split_overhead)],
    "mod=const": [(C_APPLE, "", apple_data_loop_pac_split_ia_overhead), (C_QARMA_R, "", rocket_data_loop_pac_split_ia_overhead), (C_PRINCE_R, "", rocket_data_loop_lippen_split_ia_overhead),
                  (C_QARMA_B, "", boom_data_loop_pac_split_ia_overhead), (C_PRINCE_B, "", boom_data_loop_lippen_split_ia_overhead )],
}

# REORDERED: 3rd element (Nested) moved to 1st position
right_series = [
    ("Apple M1",      C_APPLE,    "",   [apple_loop_nested_pac_overhead,  apple_loop_l_pac_overhead,   apple_loop_s_pac_overhead,    apple_deep_recursive_entry_pac_overhead,   apple_deep_recursive_return_pac_overhead]),
    ("Rocket-PAC",    C_QARMA_R,  "",   [rocket_loop_nested_pac_overhead,   rocket_loop_l_pac_overhead,   rocket_loop_s_pac_overhead,   rocket_deep_recursive_entry_pac_overhead,   rocket_deep_recursive_return_pac_overhead]),
    ("Rocket-LIPPEN", C_PRINCE_R, "",   [rocket_loop_nested_lippen_overhead,   rocket_loop_l_lippen_overhead,   rocket_loop_s_lippen_overhead,   rocket_deep_recursive_entry_lippen_overhead,   rocket_deep_recursive_return_lippen_overhead]),
    ("BOOM-PAC",      C_QARMA_B,  "",   [boom_loop_nested_pac_overhead,   boom_loop_l_pac_overhead,   boom_loop_s_pac_overhead,   boom_deep_recursive_entry_pac_overhead,   boom_deep_recursive_return_pac_overhead]),
    ("BOOM-LIPPEN",   C_PRINCE_B, "",   [boom_loop_nested_lippen_overhead,   boom_loop_l_lippen_overhead,   boom_loop_s_lippen_overhead,   boom_deep_recursive_entry_lippen_overhead,   boom_deep_recursive_return_lippen_overhead]),
    ("RAS=0, No BTB", C_PRINCE_B, "xx", [boom_custom2_loop_nested_lippen_overhead,   boom_custom2_loop_l_lippen_overhead,   boom_custom2_loop_s_lippen_overhead,   boom_custom2_deep_recursive_entry_lippen_overhead,   boom_custom2_deep_recursive_return_lippen_overhead]),
    ("RAS=0",         C_PRINCE_B, "..", [boom_custom_loop_nested_lippen_overhead,   boom_custom_loop_l_lippen_overhead,   boom_custom_loop_s_lippen_overhead,   boom_custom_deep_recursive_entry_lippen_overhead,   boom_custom_deep_recursive_return_lippen_overhead]),
]

# REORDERED labels
right_groups = [
    "Looped nested function", "Looped function-L", "Looped function_S",
    "Deep Recursive entry", "Deep Recursive return"
]

# =========================================================
#  FIGURE
# =========================================================
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(FIG_WIDTH, FIG_HEIGHT), dpi=DPI,
                                gridspec_kw={"width_ratios": [1.2, 1.2]})

GAP_SIZE = 0.04

# -------------------------------------------------------
#  LEFT SUBPLOT (NOW RETURN ADDRESS PROTECTION)
# -------------------------------------------------------
x_right = np.arange(len(right_groups))
all_edges_r = []

for g_idx, group in enumerate(right_groups):
    n = len(right_series)
    total_group_width = (n * BAR_WIDTH) + (2 * GAP_SIZE)
    start = x_right[g_idx] - total_group_width / 2
    current_offset = 0
    for s_idx, (label, col, hatch, vals) in enumerate(right_series):
        val = vals[g_idx]
        if s_idx == 1: current_offset += GAP_SIZE
        if s_idx == 3: current_offset += GAP_SIZE
        pos = start + current_offset
        disp_val = min(val, YMAX_RIGHT)
        ax1.bar(pos, disp_val, width=BAR_WIDTH, color=col, edgecolor="black", linewidth=0.5, align='edge')
        if hatch:
            ax1.bar(pos, disp_val, width=BAR_WIDTH, color="none", hatch=hatch, edgecolor="white", linewidth=0, align='edge')
        if val > YMAX_RIGHT and s_idx != 4:
            ax1.text(pos + BAR_WIDTH/2, YMAX_RIGHT + 2, str(int(val)), ha='center', va='bottom', fontsize=8, fontweight='bold')
        all_edges_r.extend([pos, pos + BAR_WIDTH])
        current_offset += BAR_WIDTH

ax1.set_xlim(min(all_edges_r) - 0.2, max(all_edges_r) + 0.2)
ax1.set_ylim(-15, YMAX_RIGHT)
ax1.set_xticks(x_right)
ax1.set_xticklabels(right_groups, rotation=20, ha="right", fontsize=XTICK_FONTSIZE)

# APPLY Y-AXIS FONT SIZE
ax1.tick_params(axis='y', labelsize=YTICK_FONTSIZE)
ax1.set_ylabel("Overhead (%)", fontsize=LABEL_FONTSIZE, fontweight='bold')
ax1.set_title("Return Address Protection", fontsize=TITLE_FONTSIZE, fontweight='bold', pad=15)
ax1.grid(axis="y", linestyle="--", alpha=0.3)
ax1.axhline(0, color='black', linewidth=0.8)

# -------------------------------------------------------
#  RIGHT SUBPLOT (NOW DATA POINTER PROTECTION)
# -------------------------------------------------------
x_left_pos = [0, 0.8, 1.8, 2.6, 3.4]
x_labels = ["zero mod", "non-zero mod", "zero mod", "non-zero mod-S", "non-zero mod"]
all_edges_l = []

for i in range(5):
    label_type = x_labels[i]
    if i < 2:
        sub_cat = left_subgroups[i]
        entries = left_data_rolled[sub_cat]
    else:
        sub_cat = left_subgroups[i - 2]
        entries = left_data_unrolled[sub_cat]

    n = len(entries)
    gap_indices = [2, 4] if label_type == "zero mod" else [1, 3]
    num_gaps = len(gap_indices)
    total_group_width = (n * BAR_WIDTH) + (num_gaps * GAP_SIZE)

    start = x_left_pos[i] - total_group_width / 2
    current_offset = 0

    for k, (col, hatch, val) in enumerate(entries):
        if k in gap_indices:
            current_offset += GAP_SIZE
        pos = start + current_offset
        disp_val = min(val, YMAX_LEFT)
        ax2.bar(pos, disp_val, width=BAR_WIDTH, color=col, edgecolor="black", linewidth=0.5, align='edge')
        if hatch:
            ax2.bar(pos, disp_val, width=BAR_WIDTH, color="none", hatch=hatch, edgecolor="white", linewidth=0, align='edge')
        if val > YMAX_LEFT:
            ax2.text(pos + BAR_WIDTH/2, YMAX_LEFT + 2, str(int(val)), ha='center', va='bottom', fontsize=8, fontweight='bold')
        all_edges_l.extend([pos, pos + BAR_WIDTH])
        current_offset += BAR_WIDTH

divider_x = (x_left_pos[1] + x_left_pos[2]) / 2
ax2.axvline(x=divider_x, color='gray', linestyle='--', linewidth=1, alpha=0.5)

ax2.text((x_left_pos[0]+x_left_pos[1])/2, YMAX_LEFT + 10, "LOOP", ha='center', fontsize=11, fontweight='bold', color="#333")
ax2.text((x_left_pos[2]+x_left_pos[4])/2, YMAX_LEFT + 10, "UNROLLED LOOP 32x", ha='center', fontsize=11, fontweight='bold', color="#333")

ax2.set_xlim(min(all_edges_l) - 0.1, max(all_edges_l) + 0.1)
ax2.set_ylim(0, YMAX_LEFT + 45)
ax2.set_xticks(x_left_pos)
ax2.set_xticklabels(x_labels, fontsize=XTICK_FONTSIZE)

# APPLY Y-AXIS FONT SIZE
ax2.tick_params(axis='y', labelsize=YTICK_FONTSIZE)

ax2.set_ylabel("Overhead (%)", fontsize=LABEL_FONTSIZE, fontweight='bold')
ax2.set_title("Data Pointer Protection", fontsize=TITLE_FONTSIZE, fontweight='bold', pad=15)
ax2.grid(axis="y", linestyle="--", alpha=0.3)

# -------------------------------------------------------
#  LEGEND
# -------------------------------------------------------
legend_elements = [
    Patch(facecolor=C_APPLE,    edgecolor="black", label="M1"),
    Patch(facecolor=C_APPLE,    hatch="..", edgecolor="white", label="M1 Fused inst."),
    Patch(facecolor=C_QARMA_R,  edgecolor="black", label="Rocket-PAC"),
    Patch(facecolor=C_PRINCE_R, edgecolor="black", label="Rocket-LIPPEN"),
    Patch(facecolor=C_QARMA_B,  edgecolor="black", label="BOOM-PAC"),
    Patch(facecolor=C_PRINCE_B, edgecolor="black", label="BOOM-LIPPEN"),
    Patch(facecolor=C_PRINCE_B, hatch="xx", edgecolor="white", label="BOOM-LIPPEN-no BTB/RAS"),
    Patch(facecolor=C_PRINCE_B, hatch="..", edgecolor="white", label="BOOM-LIPPEN-no RAS"),
]

fig.legend(handles=legend_elements, loc='upper center', ncol=4, frameon=True,
           framealpha=0.9, fontsize=LEGEND_FONTSIZE, bbox_to_anchor=(0.5, 1.07))

plt.tight_layout()
plt.subplots_adjust(top=0.78)
plt.show()
