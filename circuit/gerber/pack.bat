md order

copy /y 7G_controller.gbl     order\7G_controller.GBL
copy /y 7G_controller.gbs     order\7G_controller.GBS
copy /y 7G_controller.gbo     order\7G_controller.GBO
copy /y 7G_controller.edge    order\7G_controller.GKO
copy /y 7G_controller.gtl     order\7G_controller.GTL
copy /y 7G_controller.gts     order\7G_controller.GTS
copy /y 7G_controller.gto     order\7G_controller.GTO
copy /y 7G_controller.txt     order\7G_controller.TXT

cd order

7za a -tzip 7G_controller *.*
