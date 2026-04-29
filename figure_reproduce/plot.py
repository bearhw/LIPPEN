import json
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Patch

# =========================================================
#                    STYLE CONTROLS
# =========================================================
FIG_WIDTH, FIG_HEIGHT = 20, 5.0
DPI = 300
YMAX_LEFT  = 450.0
YMAX_RIGHT = 140.0

LABEL_FONTSIZE = 17
XTICK_FONTSIZE = 13
YTICK_FONTSIZE = 18
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
# LOAD DATA FROM JSON FILE & CALCULATE OVERHEADS
# =========================================================

# Helper function to calculate overhead
def get_ovh(val, base):
    if base == 0: return 0.0 # Prevent division by zero if evaluators leave it empty
    return ((val - base) / base) * 100.0

# Load the JSON file
try:
    with open('eval_data.json', 'r') as f:
        data = json.load(f)
except FileNotFoundError:
    print("Error: 'eval_data.json' not found. Please ensure it is in the same directory.")
    exit(1)

# 1. APPLE OVERHEADS
app = data["APPLE"]
apple_loop_l_pac_overhead = get_ovh(app["loop_l_pac"], app["LOOP_L_NOPAC"])
apple_loop_s_pac_overhead = get_ovh(app["loop_s_pac"], app["LOOP_S_NOPAC"])
apple_loop_nested_pac_overhead = get_ovh(app["loop_nested_pac"], app["LOOP_NESTED_NOPAC"])
apple_deep_recursive_return_pac_overhead = get_ovh(app["deep_recursive_return_pac"], app["DEEP_RECURSIVE_RETURN_NOPAC"])
apple_deep_recursive_entry_pac_overhead = get_ovh(app["deep_recursive_entry_pac"], app["DEEP_RECURSIVE_ENTRY_NOPAC"])
apple_data_loop_pac_fused_overhead = get_ovh(app["data_loop_pac_fused"], app["DATA_LOOP_NOPAC"])
apple_data_loop_pac_split_overhead = get_ovh(app["data_loop_pac_split"], app["DATA_LOOP_NOPAC"])
apple_data_loop_pac_split_ia_overhead = get_ovh(app["data_loop_pac_split_ia"], app["DATA_LOOP_NOPAC"])
apple_data_unrolled32_pac_fused_overhead = get_ovh(app["data_unrolled32_pac_fused"], app["DATA_UNROLLED32_NOPAC"])
apple_data_unrolled32_pac_split_overhead = get_ovh(app["data_unrolled32_pac_split"], app["DATA_UNROLLED32_NOPAC"])
apple_data_unrolled32_pac_split_const_overhead = get_ovh(app["data_unrolled32_pac_split_const"], app["DATA_UNROLLED32_NOPAC"])
apple_data_unrolled32_pac_split_reload_overhead = get_ovh(app["data_unrolled32_pac_split_reload"], app["DATA_UNROLLED32_NOPAC"])

# 2. ROCKET PAC OVERHEADS
rp = data["ROCKET_PAC"]
rocket_loop_l_pac_overhead = get_ovh(rp["loop_l_pac"], rp["LOOP_L_NOPAC"])
rocket_loop_s_pac_overhead = get_ovh(rp["loop_s_pac"], rp["LOOP_S_NOPAC"])
rocket_loop_nested_pac_overhead = get_ovh(rp["loop_nested_pac"], rp["LOOP_NESTED_NOPAC"])
rocket_deep_recursive_return_pac_overhead = get_ovh(rp["deep_recursive_return_pac"], rp["DEEP_RECURSIVE_RETURN_NOPAC"])
rocket_deep_recursive_entry_pac_overhead = get_ovh(rp["deep_recursive_entry_pac"], rp["DEEP_RECURSIVE_ENTRY_NOPAC"])
rocket_data_loop_pac_split_overhead = get_ovh(rp["data_loop_pac_split"], rp["DATA_LOOP_NOPAC"])
rocket_data_loop_pac_split_ia_overhead = get_ovh(rp["data_loop_pac_split_ia"], rp["DATA_LOOP_NOPAC"])
rocket_data_unrolled32_pac_split_overhead = get_ovh(rp["data_unrolled32_pac_split"], rp["DATA_UNROLLED32_NOPAC"])
rocket_data_unrolled32_pac_split_const_overhead = get_ovh(rp["data_unrolled32_pac_split_const"], rp["DATA_UNROLLED32_NOPAC"])
rocket_data_unrolled32_pac_split_reload_overhead = get_ovh(rp["data_unrolled32_pac_split_reload"], rp["DATA_UNROLLED32_NOPAC"])

# 3. ROCKET LIPPEN OVERHEADS
rl = data["ROCKET_LIPPEN"]
rocket_loop_l_lippen_overhead = get_ovh(rl["loop_l_lippen"], rl["LOOP_L_NOLIPPEN"])
rocket_loop_s_lippen_overhead = get_ovh(rl["loop_s_lippen"], rl["LOOP_S_NOLIPPEN"])
rocket_loop_nested_lippen_overhead = get_ovh(rl["loop_nested_lippen"], rl["LOOP_NESTED_NOLIPPEN"])
rocket_deep_recursive_return_lippen_overhead = get_ovh(rl["deep_recursive_return_lippen"], rl["DEEP_RECURSIVE_RETURN_NOLIPPEN"])
rocket_deep_recursive_entry_lippen_overhead = get_ovh(rl["deep_recursive_entry_lippen"], rl["DEEP_RECURSIVE_ENTRY_NOLIPPEN"])
rocket_data_loop_lippen_split_overhead = get_ovh(rl["data_loop_lippen_split"], rl["DATA_LOOP_NOLIPPEN"])
rocket_data_loop_lippen_split_ia_overhead = get_ovh(rl["data_loop_lippen_split_ia"], rl["DATA_LOOP_NOLIPPEN"])
rocket_data_unrolled32_lippen_split_overhead = get_ovh(rl["data_unrolled32_lippen_split"], rl["DATA_UNROLLED32_NOLIPPEN"])
rocket_data_unrolled32_lippen_split_const_overhead = get_ovh(rl["data_unrolled32_lippen_split_const"], rl["DATA_UNROLLED32_NOLIPPEN"])
rocket_data_unrolled32_lippen_split_reload_overhead = get_ovh(rl["data_unrolled32_lippen_split_reload"], rl["DATA_UNROLLED32_NOLIPPEN"])

# 4. BOOM PAC OVERHEADS
bp = data["BOOM_PAC"]
boom_loop_l_pac_overhead = get_ovh(bp["loop_l_pac"], bp["LOOP_L_NOPAC"])
boom_loop_s_pac_overhead = get_ovh(bp["loop_s_pac"], bp["LOOP_S_NOPAC"])
boom_loop_nested_pac_overhead = get_ovh(bp["loop_nested_pac"], bp["LOOP_NESTED_NOPAC"])
boom_deep_recursive_return_pac_overhead = get_ovh(bp["deep_recursive_return_pac"], bp["DEEP_RECURSIVE_RETURN_NOPAC"])
boom_deep_recursive_entry_pac_overhead = get_ovh(bp["deep_recursive_entry_pac"], bp["DEEP_RECURSIVE_ENTRY_NOPAC"])
boom_data_loop_pac_split_overhead = get_ovh(bp["data_loop_pac_split"], bp["DATA_LOOP_NOPAC"])
boom_data_loop_pac_split_ia_overhead = get_ovh(bp["data_loop_pac_split_ia"], bp["DATA_LOOP_NOPAC"])
boom_data_unrolled32_pac_split_overhead = get_ovh(bp["data_unrolled32_pac_split"], bp["DATA_UNROLLED32_NOPAC"])
boom_data_unrolled32_pac_split_const_overhead = get_ovh(bp["data_unrolled32_pac_split_const"], bp["DATA_UNROLLED32_NOPAC"])
boom_data_unrolled32_pac_split_reload_overhead = get_ovh(bp["data_unrolled32_pac_split_reload"], bp["DATA_UNROLLED32_NOPAC"])

# 5. BOOM LIPPEN OVERHEADS
bl = data["BOOM_LIPPEN"]
boom_loop_l_lippen_overhead = get_ovh(bl["loop_l_lippen"], bl["LOOP_L_NOLIPPEN"])
boom_loop_s_lippen_overhead = get_ovh(bl["loop_s_lippen"], bl["LOOP_S_NOLIPPEN"])
boom_loop_nested_lippen_overhead = get_ovh(bl["loop_nested_lippen"], bl["LOOP_NESTED_NOLIPPEN"])
boom_deep_recursive_return_lippen_overhead = get_ovh(bl["deep_recursive_return_lippen"], bl["DEEP_RECURSIVE_RETURN_NOLIPPEN"])
boom_deep_recursive_entry_lippen_overhead = get_ovh(bl["deep_recursive_entry_lippen"], bl["DEEP_RECURSIVE_ENTRY_NOLIPPEN"])
boom_data_loop_lippen_split_overhead = get_ovh(bl["data_loop_lippen_split"], bl["DATA_LOOP_NOLIPPEN"])
boom_data_loop_lippen_split_ia_overhead = get_ovh(bl["data_loop_lippen_split_ia"], bl["DATA_LOOP_NOLIPPEN"])
boom_data_unrolled32_lippen_split_overhead = get_ovh(bl["data_unrolled32_lippen_split"], bl["DATA_UNROLLED32_NOLIPPEN"])
boom_data_unrolled32_lippen_split_const_overhead = get_ovh(bl["data_unrolled32_lippen_split_const"], bl["DATA_UNROLLED32_NOLIPPEN"])
boom_data_unrolled32_lippen_split_reload_overhead = get_ovh(bl["data_unrolled32_lippen_split_reload"], bl["DATA_UNROLLED32_NOLIPPEN"])

# 6. BOOM CONFIG1 (NO RAS) OVERHEADS
bc1 = data["BOOM_CONFIG1_NO_RAS"]
boom_custom_loop_l_lippen_overhead = get_ovh(bc1["loop_l_lippen"], bc1["LOOP_L_NOLIPPEN"])
boom_custom_loop_s_lippen_overhead = get_ovh(bc1["loop_s_lippen"], bc1["LOOP_S_NOLIPPEN"])
boom_custom_loop_nested_lippen_overhead = get_ovh(bc1["loop_nested_lippen"], bc1["LOOP_NESTED_NOLIPPEN"])
boom_custom_deep_recursive_return_lippen_overhead = get_ovh(bc1["deep_recursive_return_lippen"], bc1["DEEP_RECURSIVE_RETURN_NOLIPPEN"])
boom_custom_deep_recursive_entry_lippen_overhead = get_ovh(bc1["deep_recursive_entry_lippen"], bc1["DEEP_RECURSIVE_ENTRY_NOLIPPEN"])

# 7. BOOM CONFIG2 (NO RAS, NO BTB) OVERHEADS
bc2 = data["BOOM_CONFIG2_NO_RAS_NO_BTB"]
boom_custom2_loop_l_lippen_overhead = get_ovh(bc2["loop_l_lippen"], bc2["LOOP_L_NOLIPPEN"])
boom_custom2_loop_s_lippen_overhead = get_ovh(bc2["loop_s_lippen"], bc2["LOOP_S_NOLIPPEN"])
boom_custom2_loop_nested_lippen_overhead = get_ovh(bc2["loop_nested_lippen"], bc2["LOOP_NESTED_NOLIPPEN"])
boom_custom2_deep_recursive_return_lippen_overhead = get_ovh(bc2["deep_recursive_return_lippen"], bc2["DEEP_RECURSIVE_RETURN_NOLIPPEN"])
boom_custom2_deep_recursive_entry_lippen_overhead = get_ovh(bc2["deep_recursive_entry_lippen"], bc2["DEEP_RECURSIVE_ENTRY_NOLIPPEN"])


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
