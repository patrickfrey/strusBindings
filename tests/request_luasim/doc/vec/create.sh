strusInspectVectorStorage -s path=/srv/wikipedia/storage/vec neighbor 0.8 E "david_bowie" > vec.david_bowie.json
strusInspectVectorStorage -s path=/srv/wikipedia/storage/vec neighbor 0.8 E "brian_eno" > vec.brian_eno.json
strusInspectVectorStorage -s path=/srv/wikipedia/storage/vec neighbor 0.8 E "david_bowie" + E "brian_eno"> vec.david_bowie__brian_eno.json
strusInspectVectorStorage -s path=/srv/wikipedia/storage/vec neighbor 0.8 N "collaborations"  + E "brian_eno"> vec.collaborations.json
strusInspectVectorStorage -s path=/srv/wikipedia/storage/vec neighbor 0.8 N "musician" > vec.musician.json
strusInspectVectorStorage -s path=/srv/wikipedia/storage/vec neighbor 0.8 N "album" > vec.album.json
strusInspectVectorStorage -s path=/srv/wikipedia/storage/vec neighbor 0.8 N "collaborations" + E "david_bowie" + E "brian_eno" > vec.collaborations__david_bowie__brian_eno.json
strusInspectVectorStorage -s path=/srv/wikipedia/storage/vec neighbor 0.8 N "album" + N "collaborations" + E "david_bowie" + E "brian_eno" > vec.album__collaborations__david_bowie__brian_eno.json


