# final
功能：按下鍵盤的down方向鍵後，PC端的XBee會傳送RPC command給板子上的Xbee，開始進行導航
首先，車子會先偵測直線，沿著直線走到第一個目標物（用ping量與目標物的距離，何時要停下）
接著，繞著目標物轉圈（約一圈半）停下後，會偵測Apriltag的位置，移動到apriltag上，再偵測由apriltag延伸出的直線，沿著直線走到第二個目標物

主程式的main中，主要用來接收由openmv傳來的影像資訊（直線、apriltag的位置）並且用一個eventquene來做Xbee的傳輸，
車子主要的動作由rpc function完成（cargo）其中，
mode1：第一次偵測直線＋繞圈
mode2：偵測apriltag
mode3：第二次偵測直線
mode1, 2做完後會直接進入下一個mode，mode3做完後則是return，結束，待下次又按下down方向鍵後才再次由mode1開始
（截圖是一開始按下down方向鍵後的傳輸資訊（開始動作，尋找直線），在尚未找到車子前方地面的直線前，位置設為0）
