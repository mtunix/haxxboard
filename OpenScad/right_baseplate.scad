positions2 = [
   // for: key_idx plus shift, last: key_idx + shift + half oversize
   concat([for(i=[0:5]) i+1/2], [6 + 1/2 + 1/4]),
   [for(i=[0:7]) i],
   concat([for(i=[0:5]) i+1/4], [6 + 1/4 + 3/8]),
   concat([for(i=[0:4]) i+3/4], [5 + 3/4 + 5/8]),
   [7/8, 7/8]
];
   
positions = [[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.25],
 [0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5],
 [0.75, 1.75, 2.75, 3.75, 4.75, 5.75, 7.125],
 [1.25, 2.25, 3.25, 4.25, 5.25, 6.875],
 [1.375, 2.625, 4.5, 6.875]];


difference(){ 
    cube([500, 500, 2.3]);
    for(row=[0:len(positions)-1]) {
        for(col=[0:len(positions[row])-1]){
            translate([positions[row][col]*19.05+50, 19.05*row+50, -0.1])
                cube(14.5,14.5,2.5);
        }
    }
}