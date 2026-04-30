## 간단한 슈팅 게임 프로젝트

+ 마지막으로 발사한 시간, Interval 로 자동 발사 처리
+ 샷건은 FMath::VRandCone 으로 원뿔 내 랜덤 방향 구해서 산탄 구현
+ Gun 에서 사격 시 Broadcast 뿌리고 플레이어는 바인딩한 ABP의 AddRecoil 함수 호출
+ ABP 에서 Transform (Modify) Bone 노드를 이용해서 특정 소켓 기준으로 회전값을 적용하여 반동 구현
+ 총기별로 반동 강도, 반동 회복 속도 에디터에서 수정하도록 함

