for IDX in {0..2} {5..10};
do
python3 utils/renderTiles.py outputs/case09/case09-output-$IDX.txt -o outputs/case09/case09-output-$IDX.png   
done