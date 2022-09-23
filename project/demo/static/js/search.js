$("#submit").click(() => {
  console.log("working");
  let keyValues = [];
  $("#keyField")
    .children("input")
    .each(
      (i, e) => {
        keyValues.push(e.val());
      }
    );
  console.log(keyValues);
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
