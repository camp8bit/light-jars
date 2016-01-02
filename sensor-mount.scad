
jarDiameter = 86;

lipOverhang = 20;

mountGap = 33;
mountHoleSize = 2;

thickness = 6;
bulkhead = [50, thickness, thickness];

// cylinder(jarDiameter);

cube(bulkhead, true);

support = [jarDiameter - bulkhead[0] + thickness * 2, thickness, thickness];

translate([bulkhead[0] * 0.707 + thickness, support[0] * 0.33, 0]) {
    rotate([0,0,45]){
        cube(support, true);
        translate([support[0] / 2, 0, lipOverhang / 2 - thickness / 2]){
            cube([thickness, thickness, lipOverhang], true);
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
