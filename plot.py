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
#  DATA
# =========================================================
left_subgroups = ["mod=0", "mod=const", "mod=non-zero-2"]

left_data_unrolled = {
    "mod=0": [(C_APPLE, "", 266), (C_APPLE, "..", 266), (C_QARMA_R, "", 276),
              (C_PRINCE_R, "", 237), (C_QARMA_B, "", 176), (C_PRINCE_B, "", 176)],
    "mod=const": [(C_APPLE, "", 266), (C_QARMA_R, "", 276), (C_PRINCE_R, "", 237),
                  (C_QARMA_B, "", 177), (C_PRINCE_B, "", 177)],
    "mod=non-zero-2": [(C_APPLE, "", 270), (C_QARMA_R, "", 276), (C_PRINCE_R, "", 237),
                       (C_QARMA_B, "", 179), (C_PRINCE_B, "", 169)],
}

left_data_rolled = {
    "mod=0": [(C_APPLE, "", 396), (C_APPLE, "..", 265), (C_QARMA_R, "", 78),
              (C_PRINCE_R, "", 73), (C_QARMA_B, "", 140), (C_PRINCE_B, "", 125)],
    "mod=const": [(C_APPLE, "", 426), (C_QARMA_R, "", 78), (C_PRINCE_R, "", 73),
                  (C_QARMA_B, "", 123), (C_PRINCE_B, "", 110)],
}

right_series = [
    ("Apple M1",      C_APPLE,    "",   [95,   1,   0.1,  18,   -0.5]),
    ("Rocket-PAC",    C_QARMA_R,  "",   [23,   56,  22,  18,  21]),
    ("Rocket-LIPPEN", C_PRINCE_R, "",   [23,   47,  19,  15,  18]),
    ("BOOM-PAC",      C_QARMA_B,  "",   [15,   224,  -7,  19, 158]),
    ("BOOM-LIPPEN",   C_PRINCE_B, "",   [15,   224,  -7,  19, 136]),
    ("RAS=0, No BTB", C_PRINCE_B, "xx", [37,   37,  25,   5,  19]),
    ("RAS=0",         C_PRINCE_B, "..", [ 6,  224,  17,  16,  76]),
]

right_groups = [
    "Looped function-L", "Looped function_S", "Looped nested function",
    "Deep Recursive entry", "Deep Recursive return"
]

# =========================================================
#  FIGURE
# =========================================================
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(FIG_WIDTH, FIG_HEIGHT), dpi=DPI,
                                gridspec_kw={"width_ratios": [1.2, 1.2]})

GAP_SIZE = 0.04

# -------------------------------------------------------
#  LEFT SUBPLOT
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
        ax1.bar(pos, disp_val, width=BAR_WIDTH, color=col, edgecolor="black", linewidth=0.5, align='edge')
        if hatch:
            ax1.bar(pos, disp_val, width=BAR_WIDTH, color="none", hatch=hatch, edgecolor="white", linewidth=0, align='edge')
        if val > YMAX_LEFT:
            ax1.text(pos + BAR_WIDTH/2, YMAX_LEFT + 2, str(int(val)), ha='center', va='bottom', fontsize=8, fontweight='bold')
        all_edges_l.extend([pos, pos + BAR_WIDTH])
        current_offset += BAR_WIDTH

divider_x = (x_left_pos[1] + x_left_pos[2]) / 2
ax1.axvline(x=divider_x, color='gray', linestyle='--', linewidth=1, alpha=0.5)

ax1.text((x_left_pos[0]+x_left_pos[1])/2, YMAX_LEFT + 10, "LOOP", ha='center', fontsize=11, fontweight='bold', color="#333")
ax1.text((x_left_pos[2]+x_left_pos[4])/2, YMAX_LEFT + 10, "UNROLLED LOOP 32x", ha='center', fontsize=11, fontweight='bold', color="#333")

ax1.set_xlim(min(all_edges_l) - 0.1, max(all_edges_l) + 0.1)
ax1.set_ylim(0, YMAX_LEFT + 45)
ax1.set_xticks(x_left_pos)
ax1.set_xticklabels(x_labels, fontsize=XTICK_FONTSIZE)

# APPLY Y-AXIS FONT SIZE
ax1.tick_params(axis='y', labelsize=YTICK_FONTSIZE)

ax1.set_ylabel("Overhead (%)", fontsize=LABEL_FONTSIZE, fontweight='bold')
ax1.set_title("Data Pointer Protection", fontsize=TITLE_FONTSIZE, fontweight='bold', pad=15)
ax1.grid(axis="y", linestyle="--", alpha=0.3)

# -------------------------------------------------------
#  RIGHT SUBPLOT
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
        ax2.bar(pos, disp_val, width=BAR_WIDTH, color=col, edgecolor="black", linewidth=0.5, align='edge')
        if hatch:
            ax2.bar(pos, disp_val, width=BAR_WIDTH, color="none", hatch=hatch, edgecolor="white", linewidth=0, align='edge')
        if val > YMAX_RIGHT and s_idx != 4:
            ax2.text(pos + BAR_WIDTH/2, YMAX_RIGHT + 2, str(int(val)), ha='center', va='bottom', fontsize=8, fontweight='bold')
        all_edges_r.extend([pos, pos + BAR_WIDTH])
        current_offset += BAR_WIDTH

ax2.set_xlim(min(all_edges_r) - 0.2, max(all_edges_r) + 0.2)
ax2.set_ylim(-15, YMAX_RIGHT)
ax2.set_xticks(x_right)
ax2.set_xticklabels(right_groups, rotation=20, ha="right", fontsize=XTICK_FONTSIZE)

# APPLY Y-AXIS FONT SIZE
ax2.tick_params(axis='y', labelsize=YTICK_FONTSIZE)
ax2.set_ylabel("Overhead (%)", fontsize=LABEL_FONTSIZE, fontweight='bold')
ax2.set_title("Return Address Protection", fontsize=TITLE_FONTSIZE, fontweight='bold', pad=15)
ax2.grid(axis="y", linestyle="--", alpha=0.3)
ax2.axhline(0, color='black', linewidth=0.8)

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
