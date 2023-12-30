from functools import partial
from itertools import starmap 
from pprint import pprint


def pos_from_widths(offset, widths):
  pos = [offset + widths[0]/2] + ["lololo"] * (len(widths)-1)
  for i in range(1, len(pos)):
    pos[i] = pos[i-1] + widths[i]/2 + widths[i-1]/2

  return pos

widthss_left = [
  [1]*7,
  [1.5]+[1]*5,
  [1.75]+[1]*5,
  [2.25]+[1]*5,
  [1.5, 1, 1.5, 1, 2, 1]
]

widthss_right = [
  [1]*6+[1.5],
  [1]*8,
  [1]*6+[1.75],
  [1]*5 + [2.25],
  [1.25, 1.25, 2.5, 2.25]
]

offsets_left = [0]*5
offsets_right = [0.5, 0, 0.25, 0.75, 0.75]

def main():
    xy_pos = list(starmap(pos_from_widths, zip(offsets_left, widthss_left)))
    print("left")
    pprint(xy_pos)

    print('')

    xy_pos = list(starmap(pos_from_widths, zip(offsets_right, widthss_right)))
    print("right")
    pprint(xy_pos)

if __name__ == "__main__":
  main()
