# NT219.L21.ANTN
Mật mã học 
https://docs.google.com/forms/d/e/1FAIpQLSfn0cI13yGvaEA_bc-mKvDdB_XhTvnarl0Yyu2UOuDJ3rvryA/viewanalytics
- Tên đề tài
searchable encryption 

- Các bài báo liên quan
1.  Privacy-Preserving Multi-Keyword Searchable Encryption for Distributed Systems
    DOI Bookmark: 10.1109/TPDS.2020.3027003
2.  Multi-server searchable data crypt: searchable data encryption scheme for secure distributed cloud storage
    DOI: 10.1007/s12652-020-02621-8
3. Forward Secure Searchable Symmetric Encryption
    DOI: 10.23919/ICITST.2017.8356345
-  Ngữ cảnh và ý tưởng demo
1. Ngữ cảnh
- Trapdoor là một hàm số có thể tính theo một chiều dễ dàng, được coi là rất khó tính theo chiều ngược lại nêu không có thông tin đặc biệt
- SE (mã hóa có thể tìm kiếm) là một cách đảm bảo để giữ an toàn cho dữ liệu nhạy cảm của người dùng, bên cạnh đó là khả năng tìm kiếm từ khóa từ phía máy chủ. SE cho phép tìm kiếm dữ liệu được mã hóa mà không để thông tin bị rò rỉ trong dữ liệu gốc.
Hai nhánh chính của SE là SSE (là loại mã hóa đối xứng xử dụng khóa riêng cho mỗi người dùng) và PEKS (là loại mã hóa không đối xứng sử dụng khóa công khai cho tất cả người dùng, nhưng chỉ người dùng có khóa cá nhân mới có thể tạo ra các trapdoors để tìm kiếm) 
- Mã hóa đối xứng có thể tìm kiếm (SSE) cho phép người dùng lưu trữ dữ liệu trong một biểu mẫu được mã hóa trên một semi-honest server/storage provider (ví dụ: nhà cung cấp đám mây), sao cho dữ liệu được mã hóa vẫn có thể tìm kiếm được mà không cần giải mã và không làm rò rỉ nội dung dữ liệu gốc hoặc các từ đang dược tìm kiếm cho máy chủ. Trong hầu hết các trường hợp, điều này đạt được bằng cách tạo ra một chỉ mục được mã hóa có thể tìm kiếm được, được lưu trữ cùng với dữ liệu được mã hóa (ví dụ: tài liệu) trên máy chủ. Để cho phép máy chủ truy vấn dữ liệu, máy khách tạo một trapdoor cho phép máy chủ thực hiện tìm kiếm thay mặt cho máy khách.
- Kịch bản chủ yếu bao gồm bốn thực thể: chủ sở hữu dữ liệu, người dùng dữ liệu, nhà cung cấp dịch vụ đám mây và trình tạo khóa. Trong trường hợp này, chủ sở hữu dữ liệu muốn lưu trữ một bộ tài liệu trên máy chủ đám mây vì giới hạn tài nguyên lưu trữ (hoặc bất kỳ lý do nào khác). Vì bất kỳ máy chủ nào có vấn đề với quyền riêng tư, chủ sở hữu phải mã hóa các tài liệu trước khi tải chúng lên. Dữ liệu và siêu dữ liệu được mã hóa bằng một lược đồ mật mã cho phép khả năng tìm kiếm. Nếu chủ sở hữu dữ liệu hoặc người dùng được cấp phép cần lấy tài liệu cụ thể, người dùng sẽ cần gửi một số thông tin về từ khóa truy vấn cho nhà cung cấp dịch vụ đám mây. Khi nhận được các truy vấn tìm kiếm được mã hóa từ người dùng dữ liệu, nhà cung cấp dịch vụ đám mây sẽ kiểm tra các truy vấn được mã hóa trong bộ nhớ đám mây. 
- Dữ liệu được mã hóa đáp ứng các tiêu chí tìm kiếm sẽ được truy xuất và gửi lại cho người dùng sau khi hoàn thành kiểm tra. Nhà cung cấp dịch vụ đám mây không thể tìm hiểu bất kỳ thông tin nào từ hoạt động này.
- Có hai loại SSE, tĩnh và động. SSE tĩnh là nơi dữ liệu được chuẩn bị và tải lên máy chủ lưu trữ một lần và sau đó chỉ các truy vấn tìm kiếm được thực hiện. Một SSE động cho phép thêm, xóa hoặc sửa đổi dữ liệu sau lần tải lên đầu tiên 
Nói chung, một kịch bản SSE tĩnh bao gồm sáu thuật toán thời gian đa thức (Mohamad và cộng sự 2019; Wang và cộng sự 2016):
    (1) A key generation algorithm: Keygen (1 k) được thực thi bởi máy khách. Đầu vào của nó là một tham số bảo mật k và nó xuất ra một khóa bí mật K.
    (2) A keyword index generation algorithm: BuildIndex (K, D) được thực thi bởi máy khách. Nó lấy một khóa bí mật K và một tập hợp các tài liệu D (DB ánh xạ từ khóa-tài liệu) làm đầu vào và xuất ra một chỉ mục từ khóa I.
    (3) A symmetric encryption algorithm: Encryption (D, K) được chạy bởi máy khách và thường mang tính xác suất. Nó lấy một bộ tài liệu D và các khóa và xuất ra một bộ mật mã C của các tài liệu.
    (4) A keyword trapdoor generation algorithm: Trapdoor (K, w) được thực thi bởi máy khách và thường là xác định. Đầu vào của nó là cả khóa bí mật K và từ khóa truy vấn w tạo ra cửa sập Tw làm đầu ra cho từ khóa w.
    (5) A keyword search algorithm: Search (I, Tw) được thực thi bởi máy chủ và là một thuật toán tương tác xác định. Nó nhận cả một chỉ mục từ khóa I và một cửa sập Tw làm đầu vào và đầu ra một bộ tài liệu D (w) có chứa từ khóa truy vấn w.
    (6) A symmetric decryption algorithm: Decryption (C, K) được chạy trên máy khách. Nó nhận một bộ mật mã C và khóa K làm đầu vào và đầu ra một bộ tài liệu D.

- Mặt khác, có các lược đồ SSE động bao gồm Thuật toán cập nhật lấy làm đầu vào cho tài liệu, danh sách từ khóa và tên hoạt động như thêm, xóa và sửa đổi. Thuật toán Cập nhật xuất ra một chỉ mục và bản mã mới.
Gọi D (w) biểu thị tập tài liệu chứa từ khóa w và id (d) biểu thị định danh cho tài liệu d∈D. Lược đồ SSE là đúng nếu lược đồ mã hóa đối xứng được triển khai là đúng và đối với tất cả các từ khóa w, Tìm kiếm (Trapdoor K (w), I) = {id (d) | d∈ D (w)} (Mohamad và cộng sự 2019) . 