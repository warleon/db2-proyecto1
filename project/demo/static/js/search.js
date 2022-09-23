$("#submit").submit(() => {
  let keyValues = [];
  $("#keyField")
    .children("input")
    .each(() => {
      keyValues.push($(this).val());
    });
  console.log(ek);
  const config = {
    url: "/search",
    type: "POST",
    datatype: "json",
    data: {
      user: $("#username").value(),
      pass: $("#password").value(),
      db: $("#database").value(),
      table: $("#table").value(),
      key: keyValues,
    },
    success: (data) => {
      console.log(data);

      $("#dashboard").text(data);
    },
    error: (err) => {
      console.log(err);
    },
  };
  $.ajax(config);
});

$("#addField").click(() => {
  $("#addField").before($("#addField").prev().clone());
});
