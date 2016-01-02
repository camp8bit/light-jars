
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

for (rotation = [[45, 1], [-45, -1], [-135, -1], [135, 1]]) {
    translate([bulkhead[0] * 0.707 + thickness, support[0] * 0.33, 0]) {
        rotate([0,0,45]){
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

translate([bulkhead[0] * 0.707 + thickness, -support[0] * 0.33, 0]) {
    rotate([0,0,-45]){
        cube(support, true);
        translate([support[0] / 2, 0, lipOverhang / 2 - thickness / 2]){
            cube([thickness, thickness, lipOverhang], true);
        }
    }
}

translate([-bulkhead[0] * 0.707 - thickness, -support[0] * 0.33, 0]) {
    rotate([0,0,-135]){
        cube(support, true);
        translate([support[0] / 2, 0, lipOverhang / 2 - thickness / 2]){
            cube([thickness, thickness, lipOverhang], true);
        }
    }
}

translate([-bulkhead[0] * 0.707 - thickness, support[0] * 0.33, 0]) {
    rotate([0,0,135]){
        cube(support, true);
        translate([support[0] / 2, 0, lipOverhang / 2 - thickness / 2]){
            cube([thickness, thickness, lipOverhang], true);
        }
    }
}
