
jarDiameter = 86;

lipOverhang = 20;
proudness = 5;

mountGap = 33;
mountHoleSize = 2;

thickness = 6;
bulkhead = [50, thickness, thickness];

// cylinder(jarDiameter);

cube(bulkhead, true);

support = [jarDiameter - bulkhead[0] + thickness * 2, thickness, thickness];

for (params = [[45, 1, 1], [-45, -1, 1], [-135, -1, -1], [135, 1, -1]]) {
    translate([params[2] * (bulkhead[0] * 0.707 + thickness), params[1] * support[0] * 0.33, 0]) {
        rotate([0,0,params[0]]){
            cube(support, true);
            translate([support[0] / 2, 0, 0]){
                translate([-thickness / 2, 0, proudness]){
                    cube([thickness, thickness, proudness], true);
                }
                
                translate([0, 0, lipOverhang / 2 - thickness / 2]){
                    difference() {
                        cylinder(lipOverhang, thickness / 2, thickness / 2, true);
                        translate([-thickness * 0.6, 0, 0]){
                            cube([thickness, thickness, lipOverhang * 2], true);
                        }
                    }
                }
            }
        }
    }
}

