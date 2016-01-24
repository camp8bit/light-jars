/*

    This is a mounting plate that sits over a speaker mounted in the jar lid, and 
    lets to mount the PIR sensor through the two screw holes so that the PIR
    faces straight down in the direction the speaker faces.
    
    Todo:
        * Put the camp 8 bit space invader on the design somewhere

    Author:
        Ben Nolan <bnolan@gmail.com>
        
*/

lidDiameter = 43;
lidDepth = 16;
lipOverhang = 27;
proudness = 15;

mountGap = 33;
mountHoleSize = 1.5;

thickness = 6;
bulkhead = [28, thickness, thickness];

// Undersizing to get a press tight fit
pressTightness = 0.5;

translate([0, 0, proudness + thickness / 2]){
    %cylinder(lidDepth, lidDiameter, lidDiameter);
}

difference () {
    cube(bulkhead, true);
    
    union() {
        translate([mountGap / 2, 0, 0]){
            cylinder(thickness * 2, mountHoleSize, mountHoleSize, true);
        }
        translate([-mountGap / 2, 0, 0]){
            cylinder(thickness * 2, mountHoleSize, mountHoleSize, true);
        }
    }
}

support = [lidDiameter - bulkhead[0] * (1 - sin(45)) - pressTightness, thickness, thickness];

clampRotation = 14;

for (params = [
        [45, 1, 1, -clampRotation], 
        [-45, -1, 1, clampRotation], 
        [-135, -1, -1, -clampRotation], 
        [135, 1, -1, clampRotation]
    ]) {
    translate([params[2] * (bulkhead[0] - thickness / 2), params[1] * support[0] * 0.33, 0]) {
        rotate([0,0,params[0]]){
            cube(support, true);
            translate([support[0] / 2, 0, 0]){
                translate([-thickness / 2, 0, proudness * 0.5]){
                    cube([thickness, thickness, proudness], true);
                }
                
                translate([0, 0, lipOverhang / 2 - thickness / 2]){
                    difference() {
                        cylinder(lipOverhang, thickness / 2, thickness / 2, true);
                        translate([-thickness * 0.6, 0, 0]){
                            rotate([0,0,params[3]]){
                                cube([thickness, thickness * 2, lipOverhang * 2], true);
                            }
                        }
                    }
                }
            }
        }
    }
}

