$("#mainForm").submit(() => {
	const config = {
		url: "",
		type: "",
		datatype: "json",
		data: {
		},
		success: (data) => {
		},
		error: (err) => { console.log(err); }
	};
	$.ajax(config);
}); 