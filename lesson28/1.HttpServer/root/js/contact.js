document.getElementById('contact-form').addEventListener('submit', function(event) {
	            event.preventDefault();
            const name = document.getElementById('name').value.trim();
            const email = document.getElementById('email').value.trim();
            const message = document.getElementById('message').value.trim();
            
            if (!name) {
                alert('请输入您的姓名');
                return;
            }
            
             const emailPattern = /^[a-zA-Z0-9._-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,6}$/; //匹配由大小写字母、数字、点（.）、下划线（_）、连字符（-）组成的字符串
            if (!email || !emailPattern.test(email)) {
                alert('请输入有效的邮箱地址');
                return;
            }
            
            const phonePattern = /^\d{10,15}$/;//匹配由 10 到 15 个数字组成的字符串
            if (!message || !phonePattern.test(message)) {
                alert('请输入有效的电话信息');
                return;
            }

            alert('信息提交成功');
            document.getElementById('contact-form').reset();
        });
		
		
		const locationLink = document.getElementById('location');
		const mapFrame = document.getElementById('map-frame');
		const mapContainer1 = document.getElementById('map-container-1');
		const mapContainer2 = document.getElementById('map-container-2');
		
		locationLink.addEventListener('click', function(event) {
		  event.preventDefault(); // 阻止默认链接行为
		
		  // 更新 iframe 的 src 属性，使用你需要显示的地点的经纬度
		  const latitude = 30.209526;
		  const longitude = 115.020894;
		  const embedUrl = `https://www.google.com/maps/embed?pb=!1m18!1m12!1m3!1d346128.7730250731!2d${longitude}!3d${latitude}!2m3!1f0!2f0!3f0!3m2!1i1024!2i768!4f13.1!3m3!1m2!1s0x0%3A0x0!2zMMwwMDCsMzknMjAuMSJTIDMwwrAwOSc1LjYiRQ!5e0!3m2!1sen!2sus!4v1624060887483`;
		
		  // 迭代地图
		  mapContainer1.style.display = 'none';
		  mapContainer2.style.display = 'block';
		
		  // 更新地图
		  mapFrame.src = embedUrl;
		});