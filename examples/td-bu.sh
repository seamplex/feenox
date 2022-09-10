for i in $(seq -w 0 71); do
 echo $i
 montage -mode concatenate -tile 2x1 maze-tran-td-0${i}.png maze-tran-bu-0${i}.png maze-td-bu-0${i}.png
done

ffmpeg -y -framerate 20 -f image2 -i maze-td-bu-%%03d.png maze-td-bu.mp4
ffmpeg -y -framerate 20 -f image2 -i maze-td-bu-%%03d.png maze-td-bu.gif
