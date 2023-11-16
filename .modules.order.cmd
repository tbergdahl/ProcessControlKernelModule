cmd_/home/trenton/CPTS360/Lab4/modules.order := {   echo /home/trenton/CPTS360/Lab4/kmlab.ko; :; } | awk '!x[$$0]++' - > /home/trenton/CPTS360/Lab4/modules.order
